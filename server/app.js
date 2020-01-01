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
var savedData;
io.on('connection', function (socket) {
    
      
    socket.on("isControlSocket", function (data) {
        if (data) {
            
         console.log(data);
         
            arduinoSocket = socket;
            socket.emit("arduinoConnected",1);
            
        } else {
            
            userSockets.add(socket);
            socket.emit("initSocket",savedData);
        }
    });
   
    socket.on("controlUpdate", function (data) {
        if(data){
            console.log(data);
            socket.broadcast.emit("controlUpdate",data);
            if(data.init){
                savedData = data;
                delete savedData.init
            }
            if(savedData){
                if(data.ph){
                    savedData.ph = data.ph;
                }
                if(data.wt){
                    savedData.wt = data.wt;
                }
                if(data.iat){
                    savedData.iat = data.iat;
                }
                if(data.oat){
                    savedData.oat = data.oat;
                }
                if(data.oh){
                    savedData.oh = data.oh;
                }
                if(data.tv){
                    savedData.tv = data.tv;
                }
                if(data.tds){
                    savedData.tds = data.tds;
                }
                if(data.wl){
                    savedData.wl = data.wl;
                }
            }
        }
    });
    socket.on("WantedpH",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedpH",data);
            if(savedData){
                savedData.wph = data;
            }
            
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
    socket.on("WantedHumidity",function(data){ //todo
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedHumidity",data);
            if(savedData){
                savedData.wh = data;
            }
            
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
    socket.on("WantedPumpInterval",function(data){ //todo
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedPumpInterval",data);
            if(savedData){
                savedData.wpi = data;
            }
            
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
    socket.on("WaterDrainTime",function(data){ //todo
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WaterDrainTime",data);
            if(savedData){
                savedData.wdi = data;
            }
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
            if(savedData){
                savedData.dsh = data;
            } 
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
            if(savedData){
                savedData.wpi = data;
            }
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
            if(savedData){
                savedData.wti = data;
            }
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
    socket.on("WantedAirSensorInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedAirSensorInterval",data);
            if(savedData){
                savedData.asi = data;
            } 
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "WantedAirSensorInterval"
            })
        }
    });
    socket.on("WantedTDSInterval",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            io.emit("WantedTDSInterval",data);
            if(savedData){
                savedData.tdsi = data;
            }
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
            if(savedData){
                savedData.ti = data;
            }
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
            if(arduinoSocket){
                arduinoSocket.emit("CalibrateLowerpH",data);
            }
            else{
                socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: "Arduino Not Connected",
                field: "CalibrateLowerpH"
            });
        
            }
             
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "CalibrateLowerpH"
            });
        }
    });
    socket.on("CalibrateMiddlepH",function(data){
         //TODO filter data
         if(typeof(data)=='number'){
            if(arduinoSocket){
                arduinoSocket.emit("CalibrateMiddlepH",data);
            }
            else{
                socket.emit("response",{
                    success:0,
                    fromArduino: 0,
                    message: "Arduino Not Connected",
                    field: "CalibrateMiddlepH"
                });
            }
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
             if(arduinoSocket){
                arduinoSocket.emit("CalibrateUpperpH",data);
             }
             else{
                socket.emit("response",{
                    success:0,
                    fromArduino: 0,
                    message: "Arduino Not Connected",
                    field: "CalibrateUpperpH"
                });
             }
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
            if(arduinoSocket){
                arduinoSocket.emit("CalibrateTDS",data);
            }
            else{
                socket.emit("response",{
                    success:0,
                    fromArduino: 0,
                    message: "Arduino Not Connected",
                    field: "CalibrateTDS"
                })
            }
            
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
            if(savedData){
                savedData.wwl = data;
            }
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
            if(savedData){
                savedData.wca = data;
            }
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
        if(typeof(data)=='number' && (data>=0 && data<=100)){
            io.emit("WantedAutomicBrightness",data);
            if(savedData){
                savedData.wab = data;
            }   
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
            if(savedData){
                savedData.pt = data;
            }    
        }
        else{
            socket.emit("response",{
                success:0,
                fromArduino: 0,
                message: WrongDataTypeMessage + typeof(data),
                field: "pHTolerance"
            });
        }
    });
   socket.on("disconnect",function(){
       
       if(arduinoSocket && socket.id == arduinoSocket.id){
           console.log("d");
           arduinoSocket = null;
           socket.broadcast.emit("ArduinoDisconnected",1);
       }
   });
});

http.listen(3000, function () {
    console.log('listing on port 80');
});

