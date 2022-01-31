const express = require('express');
//const path = require('path');
const app=express();
const {port} = require('./config');
//const ejs = require('ejs');

//Dodanie funkcji js i zdjęć do zwracanych przez router podstron
app.use(express.static('Frontend'));

//db
require('./db/mongoose');
//app.set('view engine','ejs');


//routes
const apiRouter = require('./routes/api');
app.use('/', apiRouter);




//server
app.listen(port,function(){
    console.log('serwer słucha... http://localhost:'+port);
})