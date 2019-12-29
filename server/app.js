const keys = require("./config/keys")


const express = require("express");
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const ArrayList = require('arraylist');
const mongoose = require('mongoose');

const WrongDataTypeMessage = "Wrong Data Type: ";
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
         if(typeof(data)=='number'){
            io.emit("WantedpH",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedpH"
            })
        }
    });
    socket.on("WantedHumidity",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedHumidity",data);
        }
        else{
            socket.emit("response",{
                success:0,    
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedHumidity"
            })
        }
    });
    socket.on("WantedPumpInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedPumpInterval",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedPumpInterval"
            })
        }
    });
    socket.on("WaterDrainTime",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WaterDrainTime",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WaterDrainTime"
            })
        }
    });
    socket.on("DistanceSensorHeight",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("DistanceSensorHeight",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "DistanceSensorHeight"
            })
        }
    });
    socket.on("WantedpHInterval",function(data){
    //TODO filter data'
        if(typeof(data)=='number'){
            io.emit("WantedpHInterval",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedpHInterval"
            })
        }
    });
    socket.on("WantedWaterTemperatureInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedWaterTemperatureInterval",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedWaterTemperatureInterval"
            })
        }
    });
    socket.on("WantedAirTemperatureInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedAirTemperatureInterval",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedAirTemperatureInterval"
            })
        }
    });
    socket.on("WantedTDSInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedTDSInterval",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedTDSInterval"
            })
        }
    });
    socket.on("WantedTurbidityInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedTurbidityInterval",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedTurbidityInterval"
            })
        }
    });
    socket.on("CalibrateLowerpH",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("CalibrateLowerpH",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "CalibrateLowerpH"
            })
        }
    });
    socket.on("CalibrateMiddlepH",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("CalibrateMiddlepH",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "CalibrateMiddlepH"
            })
        }
    });
    socket.on("CalibrateUpperpH",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("CalibrateUpperpH",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "CalibrateUpperpH"
            })
        }
    });
    socket.on("CalibrateTDS",function(data){
        //TODO filter data
        if(typeof(data)=='number'){
            io.emit("CalibrateTDS",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "CalibrateTDS"
            })
        }
    });
    socket.on("WantedWaterLevel",function(data){
        //TODO filter data
        if(typeof(data)=='number'){
            io.emit("WantedWaterLevel",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedWaterLevel"
            })
        }
    });

    socket.on("WaterContainerArea",function(data){
        //TODO filter data
        if(typeof(data)=='number'){
            io.emit("WaterContainerArea",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WaterContainerArea"
            })
        }
    });

    socket.on("WantedAutomaticBrightness",function(data){
        //TODO filter data
        if(typeof(data)=='number'){
            io.emit("WantedAutomicBrightness",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedAutomcaticBrightness"
            })
        }
    });
    socket.on("pHTolerance",function(data){
        if(typeof(data)=='number'){
            io.emit("phTolerance",data);
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "pHTolerance"
            })
        }
        
           /* arduinoSocket.emit("pHTolerance",data);
            
            for(int i = 0; i<userSockets.length; i++){
                userSockets.get(i).emit("pHTolerance",data);
                userSockets.get(i).emit("response",{
                    success: 0,
                    fromArduino: 1,

                })
            }
       
        }*/
        
        
    });



   
   socket.on("disconnect",function(){
       console.log("d");
       if(socket.id == arduinoSocket.id){
           arduinoSocket = null;
           socket.broadcast.emit("ArduinoDisconnected",true);
       }
   })
})

http.listen(3000, function () {
    console.log('listing on port 80');
});

function initUserSocket(userSocket)
{

}