
const { db } = require('mongodb');

const mongoose = require('mongoose');

//db connect
mongoose.connect('mongodb://127.0.0.1:27017/dane',{
    useNewUrlParser: true,
    useUnifiedTopology: true
});



