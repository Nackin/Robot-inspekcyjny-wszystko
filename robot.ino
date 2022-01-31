#include "DHT.h"
#include <ESP32Servo.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define SOUND_SPEED 0.034 // (cm/uS)
#define DHTPIN 22
#define DHTTYPE DHT22

Servo myservo;
DHT dht(DHTPIN, DHTTYPE);

#include <WiFi.h>

IPAddress staticIP876_101(192,168,17,101);
IPAddress gateway876_101(192,168,17,1);
IPAddress subnet876_101(255,255,255,0);
WiFiServer server(80);
WiFiClient client;

int task;
int x;
int ppos = 90;
int pos;
int ind = 0;
String myresultat;
String ClientRequest;
static const int servoPin = 21;
String moves[10];
String slowo;

// 1 - Prawy tył
// 2 - Lewy tył 
// 3 - Prawy przód
// 4 - Lewy przód
const int dirPin1 = 32;
const int stepPin1 = 33;
const int dirPin2 = 25;
const int stepPin2 = 26;
const int dirPin3 = 12;
const int stepPin3 = 13;
const int dirPin4 = 27;
const int stepPin4 = 14;
const int stepsPerRevolution = 200;

// Front - 1
// Right - 2
// Back - 3
// Left - 4
const int TrigPin1 = 18;
const int EchoPin1 = 19;
const int TrigPin2 = 17;
const int EchoPin2 = 5;
const int TrigPin3 = 4;
const int EchoPin3 = 16;
const int TrigPin4 = 15;
const int EchoPin4 = 2;
long duration1;
float distanceCm1;
long duration2;
float distanceCm2;
long duration3;
float distanceCm3;
long duration4;
float distanceCm4;
float hum;
float temp;

String ReadIncomingRequest(){
  while(client.available()) {
    ClientRequest = (client.readStringUntil('\r'));
    if ((ClientRequest.indexOf("HTTP/1.1")>0)&&(ClientRequest.indexOf("/favicon.ico")<0)){
      myresultat = ClientRequest;
    }
  }
  return myresultat;
}

void setup()
{
  Serial.begin(9600);
  WiFi.disconnect();
  delay(3000);
  Serial.println("START");
  WiFi.begin("Siec_3w1","3w1_projekcik");
  while ((!(WiFi.status() == WL_CONNECTED))){
    delay(300);
    Serial.print("..");
  }
  Serial.println("Connected");
  WiFi.config(staticIP876_101, gateway876_101, subnet876_101);
  Serial.println("Your static IP is");
  Serial.println((WiFi.localIP()));
  server.begin();
  dht.begin();
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
  pinMode(TrigPin1, OUTPUT);
  pinMode(EchoPin1, INPUT);
  pinMode(TrigPin2, OUTPUT);
  pinMode(EchoPin2, INPUT);
  pinMode(TrigPin3, OUTPUT);
  pinMode(EchoPin3, INPUT);
  pinMode(TrigPin4, OUTPUT);
  pinMode(EchoPin4, INPUT);
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(stepPin3, OUTPUT);
  pinMode(dirPin3, OUTPUT);
  pinMode(stepPin4, OUTPUT);
  pinMode(dirPin4, OUTPUT);
  ClientRequest = "";
}

void Servo_Right()
{
  if(ppos > 10) {
    pos = ppos-10;
    for (x = 10; x > 0; x--) {
      myservo.write(pos);
      delay(50);
      pos--; 
    }   
  }
  ppos = pos;
}

void Servo_Left()
{
  if(ppos < 170) {
    pos = ppos+10;
    for (x = 0; x < 10; x++) {
      myservo.write(pos);
      delay(50);
      pos++;
    }   
  }
  ppos = pos;
}

void Temperatura()
{
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(hum) || isnan(temp))
  {
    Serial.println(F("Blad odczytu!"));
    return;
  }
  delay(20);
}

void Odo_front()
{
  digitalWrite(TrigPin1, LOW);  // Clears the trigPin
  delayMicroseconds(2);
  digitalWrite(TrigPin1, HIGH); // Sets the trigPin on HIGH state for 10 micro seconds
  delayMicroseconds(10);
  digitalWrite(TrigPin1, LOW);
  duration1 = pulseIn(EchoPin1, HIGH);  // Reads the echoPin, returns the sound wave travel time in microseconds
  distanceCm1 = duration1 * SOUND_SPEED/2;  // Calculate the distance
}

void Odo_right()
{
  digitalWrite(TrigPin2, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin2, LOW);
  duration2 = pulseIn(EchoPin2, HIGH);
  distanceCm2 = duration2 * SOUND_SPEED/2;
}

void Odo_back()
{
  digitalWrite(TrigPin3, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin3, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin3, LOW);
  duration3 = pulseIn(EchoPin3, HIGH);
  distanceCm3 = duration3 * SOUND_SPEED/2;
}

void Odo_left()
{
  digitalWrite(TrigPin4, LOW);
  delayMicroseconds(2);
  digitalWrite(TrigPin4, HIGH);
  delayMicroseconds(10);
  digitalWrite(TrigPin4, LOW);
  duration4 = pulseIn(EchoPin4, HIGH);
  distanceCm4 = duration4 * SOUND_SPEED/2;
}

void Sensors_odo()
{
  Odo_front();
  delay(20);
  Odo_right();
  delay(20);
  Odo_back();
  delay(20);
  Odo_left();
  delay(20);
}

void StepHigh()
{
  digitalWrite(stepPin1, HIGH);
  digitalWrite(stepPin2, HIGH);
  digitalWrite(stepPin3, HIGH);
  digitalWrite(stepPin4, HIGH);
}

void StepLow()
{
  digitalWrite(stepPin1, LOW);
  digitalWrite(stepPin2, LOW);
  digitalWrite(stepPin3, LOW);
  digitalWrite(stepPin4, LOW);  
}

void forward()
{
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, LOW);
  digitalWrite(dirPin4, HIGH);
  for(x = 0; x < 2*stepsPerRevolution; x++) {
    StepHigh();
    delayMicroseconds(2500);
    StepLow();
    delayMicroseconds(2500);
  }
  moves[ind] = "backward_1";
}

void backward()
{
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, HIGH);
  digitalWrite(dirPin4, LOW);
  for(x = 0; x < 2*stepsPerRevolution; x++) {
    StepHigh();
    delayMicroseconds(2500);
    StepLow();
    delayMicroseconds(2500);
  }
  moves[ind] = "forward_1";
}

void right()
{
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, HIGH);
  digitalWrite(dirPin4, HIGH);
  for(x = 0; x < 2*stepsPerRevolution; x++) {
    StepHigh();
    delayMicroseconds(4000);
    StepLow();
    delayMicroseconds(4000);
  }
  moves[ind] = "left_1";
}

void left()
{
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, LOW);
  digitalWrite(dirPin4, LOW); 
  for(x = 0; x < 2*stepsPerRevolution; x++) {
    StepHigh();
    delayMicroseconds(4000);
    StepLow();
    delayMicroseconds(4000);
  }
  moves[ind] = "right_1";
}

void left_rotate()
{
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, LOW);
  digitalWrite(dirPin4, LOW); 
  for(x = 0; x < (1.5)*stepsPerRevolution; x++) {
    StepHigh();
    delayMicroseconds(4000);
    StepLow();
    delayMicroseconds(4000);
  }
  moves[ind] = "right_rot_1";
}

void right_rotate()
{
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, HIGH);
  digitalWrite(dirPin4, HIGH); 
  for(x = 0; x < (1.5)*stepsPerRevolution; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
    digitalWrite(stepPin4, HIGH);
    delayMicroseconds(4000);
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin4, LOW);
    delayMicroseconds(4000);
  }
  moves[ind] = "left_rot_1";
}

void right_forward()
{
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, LOW);
  digitalWrite(dirPin4, HIGH); 
  for(x = 0; x < (1.5)*stepsPerRevolution; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
    digitalWrite(stepPin4, HIGH);
    delayMicroseconds(3500);
    StepLow();
    delayMicroseconds(3500);
  }
  moves[ind] = "lb_diag_1";
}

void left_backward()
{
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, HIGH);
  digitalWrite(dirPin4, LOW);
  for(x = 0; x < (1.5)*stepsPerRevolution; x++) {
    digitalWrite(stepPin1, HIGH);
    digitalWrite(stepPin2, LOW);
    digitalWrite(stepPin3, LOW);
    digitalWrite(stepPin4, HIGH);
    delayMicroseconds(3500);
    StepLow();
    delayMicroseconds(3500);
  }
  moves[ind] = "rf_diag_1";
}

void left_forward()
{
  digitalWrite(dirPin1, LOW);
  digitalWrite(dirPin2, HIGH);
  digitalWrite(dirPin3, LOW);
  digitalWrite(dirPin4, HIGH); 
  for(x = 0; x < (1.5)*stepsPerRevolution; x++) {
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin4, LOW);
    delayMicroseconds(3500);
    StepLow();
    delayMicroseconds(3500);
  }
  moves[ind] = "rb_diag_1";
}

void right_backward()
{
  digitalWrite(dirPin1, HIGH);
  digitalWrite(dirPin2, LOW);
  digitalWrite(dirPin3, HIGH);
  digitalWrite(dirPin4, LOW); 
  for(x = 0; x < (1.5)*stepsPerRevolution; x++) {
    digitalWrite(stepPin1, LOW);
    digitalWrite(stepPin2, HIGH);
    digitalWrite(stepPin3, HIGH);
    digitalWrite(stepPin4, LOW);
    delayMicroseconds(3500);
    StepLow();
    delayMicroseconds(3500);
  }
  moves[ind] = "lf_diag_1";
}

void Return()
{
  if ( ind > 0 ) {
    for (int i = ind; i > 0; i--) {
      slowo = moves[ind];
      if (slowo == "forward_1")   { forward(); }
      if (slowo == "backward_1")  { backward(); }
      if (slowo == "left_1")      { left(); }
      if (slowo == "right_1")     { right(); }
      if (slowo == "right_rot_1") { right_rotate(); }
      if (slowo == "left_rot_1")  { left_rotate(); }
      if (slowo == "rf_diag_1")   { right_forward(); }
      if (slowo == "rb_diag_1")   { right_backward(); }
      if (slowo == "lf_diag_1")   { left_forward(); }
      if (slowo == "lb_diag_1")   { left_backward(); }
      ind--;
      delay(100);
    }    
  }
  ind = 0;
}

void Create_doc()
{
  DynamicJsonDocument doc (1024);
  doc["tmp"] = temp;
  doc["wilg"] = hum;
  doc["od_front"] = distanceCm1;
  doc["od_right"] = distanceCm2;
  doc["od_back"] = distanceCm3;
  doc["od_left"] = distanceCm4;
  HTTPClient http;
  http.begin("192.168.17.100", 3000);
  http.addHeader("Content-Type", "application/json");
  String json;
  size_t JSONlength = serializeJson(doc, json);
  Serial.println(json);
  int httpCode = http.sendRequest("POST", json);
  Serial.printf("[HTTP] POST code: %d\n", httpCode);
}

//*********************************************************************************************************

void loop()
{
    client = server.available();
    if (!client) { return; }
    while(!client.available()) { delay(1); }
    
    if (client.available() > 0) {
      ClientRequest = ReadIncomingRequest();
      ClientRequest.remove(0, 5);
      ClientRequest.remove(ClientRequest.length()-9,9);
      if (ClientRequest == "forwardleft")   { task = 1; }
      if (ClientRequest == "forward")       { task = 2; }
      if (ClientRequest == "forwardright")  { task = 3; }
      if (ClientRequest == "left")          { task = 4; }
      if (ClientRequest == "right")         { task = 5; }
      if (ClientRequest == "backwardleft")  { task = 6; }
      if (ClientRequest == "backward")      { task = 7; }
      if (ClientRequest == "backwardright") { task = 8; }
      if (ClientRequest == "leftrotate")    { task = 9; }
      if (ClientRequest == "rightrotate")   { task = 10; }
      if (ClientRequest == "stop")          { task = 11; }
      if (ClientRequest == "sensors")       { task = 12; }
      if (ClientRequest == "servoleft")     { task = 13; }
      if (ClientRequest == "servoright")    { task = 14; }
      if (ClientRequest == "return")        { task = 15; }
    }
    if (task==1 || task==2 || task==3 || task==4 || task==5 || task==6 || task==7 || task==8 || task==9 || task==10) { ind++; }
    if (task == 1)  { left_forward(); }
    if (task == 2)  { forward(); }
    if (task == 3)  { right_forward(); }
    if (task == 4)  { left(); }
    if (task == 5)  { right(); }
    if (task == 6)  { left_backward(); }
    if (task == 7)  { backward(); }
    if (task == 8)  { right_backward(); }
    if (task == 9)  { left_rotate(); }
    if (task == 10) { right_rotate(); }
    if (task == 11) { StepLow(); }
    if (task == 12) { 
      Sensors_odo();
      Temperatura();
      Create_doc();
    }
    if (task == 13) { Servo_Left(); }
    if (task == 14) { Servo_Right(); }
    if (task == 15) { Return(); }
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("OK");
    client.println("</html>");
    client.stop();
    delay(1);
    client.flush();  
}
