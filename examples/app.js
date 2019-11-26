const keys = require("./config/keys");
const express = require("express");
const app = express();
const Light = require("./models/light-model")

//const scannerRoutes = require("./routes/scannerRoutes");
const mongoose = require('mongoose');
const createFunctions = require("./config/createFunctions");
const http = require('http').Server(app);
const scannerRoutes = require("./routes/scannerRoutes");
const axios = require("axios")
mongoose.connect(keys.mongoDB.dbURI, {useNewUrlParser: true},function(error){
    if(error)
{
    console.log(error);
}
else{
    console.log("Connected to Database");
}  
 
});


app.use(express.static( __dirname + '/public'));



app.use('/scanner',scannerRoutes);

axios.get("https://cloud.boltiot.com/remote/" + keys.Bolt.apiKey + "/serialBegin?baud=9600&deviceName=" +keys.Bolt.lockControl).then(function(req,res){
          console.log(req.data);
});
  


http.listen(8080, function() {
    console.log('app now listening');
  });