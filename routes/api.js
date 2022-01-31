const express = require('express');
const router = express.Router();
//const path = require('path');
const app=express();
const bodyParser = require('body-parser')
const ejs = require('ejs');

const jsonParser = bodyParser.json()

//db
require('../Frontend/timer')
require('../db/mongoose');
app.set('view engine','ejs');
const mongoose = require('mongoose');
const { urlencoded } = require('body-parser');
const dane = mongoose.model('dane', {
       date: { type: Date, default: new Date },
       tmp: Number,
       wilg: Number,
       od_front: Number,
       od_right: Number,
       od_left: Number,
       od_back: Number,
   
   });


//Strona główna html
//router.get('/', function(req, res) {
//       res.sendFile(path.join(__dirname, '../Frontend/index.html'));
//});

//zapisywanie danych

//router.post('/save', daneActions.saveDane);
       
//const dane = require('../db/mongoose/dane')

router.post('/', jsonParser,function(req,res,){
       const tmp =  req.body.tmp;
       const wilg = req.body.wilg;
       const od_front=req.body.od_front;
       const od_right=req.body.od_right;
       const od_left=req.body.od_left;
       const od_back=req.body.od_back;
       const newDane= new dane({
            tmp: tmp,
            wilg: wilg,
            od_front: od_front,
            od_right: od_right,
            od_left: od_left,
            od_back: od_back,
        });
       newDane.save();
       res.status(201).json(newDane);
    })





router.get('/home',(req, res) => {
       dane.find({},function(err, danex){
              res.render('indexx.ejs',{
                     daneList: danex
              })
       }).sort({date: -1}).limit(1);
})

router.get('/db',(req, res) => {
       dane.find({},function(err, danes){
              res.render('showdb.ejs',{
                     daneList: danes
              })
       }).sort({date: -1})
})


module.exports = router;
