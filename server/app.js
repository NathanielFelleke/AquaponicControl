const keys = require("./config/keys")


const express = require("express");
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const ArrayList = require('arraylist');
const mongoose = require('mongoose');

mongoose.connect(keys.mongoDB.dbURI, {useNewUrlParser: true},function(error){
    if(error)
{
    console.log(error);
}
else{
    console.log("Connected to Database");
}  
 
});

app.get('/public', function (req, res) {
    res.sendFile(__dirname + '/public/index.html');
});

app.use(express.static('public'));
var arduinoSocket;
var userSockets = new ArrayList;
io.on('connection', function (socket) {
    
    console.log("c");
    socket.on("isControlSocket", function (data) {
        if (data) {
            console.log(data);
            arduinoSocket = socket;
            
        } else {
            
            userSockets.add(socket);
            initSocket(socket);
        }
    });
   
    socket.on("controlUpdate", function (data) {
        console.log("h");
        if(data){
            
            console.log(data);
            for(var i =0; i<userSockets.length; i++){
                userSockets.get(i).emit("controlUpdate",data);
            }
            //if(data.ph)
        }
    
    });
    socket.on("WantedpH",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedpH",data);
    });
    socket.on("WantedHumidity",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedHumidity",data);
    });
    socket.on("WantedPumpInterval",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedPumpInterval",data);
    });
    socket.on("WaterDrainTime",function(data){
         //TODO filter data
        arduinoSocket.emit("WaterDrainTime",data);
    });
    socket.on("DistanceSensorHeight",function(data){
         //TODO filter data
        arduinoSocket.emit("DistanceSensorHeight",data);
    });
    socket.on("WantedpHInterval",function(data){'
    //TODO filter data'
        arduinoSocket.emit("WantedpHInterval",data);
    });
    socket.on("WantedWaterTemperatureInterval",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedWaterTemperatureInterval",data);
    });
    socket.on("WantedAirTemperatureInterval",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedAirTemperatureInterval",data);
    });
    socket.on("WantedTDSInterval",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedTDSInterval",data);
    });
    socket.on("WantedTurbidityInterval",function(data){
         //TODO filter data
        arduinoSocket.emit("WantedTurbidityInterval",data);
    });
    socket.on("CalibrateLowerpH",function(data){
         //TODO filter data
        arduinoSocket.emit("CalibrateLowerpH",data);
    });
    socket.on("CalibrateMiddlepH",function(data){
         //TODO filter data
        arduinoSocket.emit("CalibrateMiddlepH",data);
    });
    socket.on("CalibrateUpperpH",function(data){
         //TODO filter data
        arduinoSocket.emit("CalibrateUpperpH",data);
    });
    socket.on("CalibrateTDS",function(data){
        //TODO filter data
        arduinoSocket.emit("CalibrateTDS",data);
    });
   
})

http.listen(3000, function () {
    console.log('listing on port 80');
});

function initUserSocket(userSocket)
{

}