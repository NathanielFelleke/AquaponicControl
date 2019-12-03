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
    socket.on("wantedpH", function (data) {
        arduinoSocket.emit("wantedpH", data);
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
})

http.listen(3000, function () {
    console.log('listing on port 80');
});

function initUserSocket(userSocket)
{

}