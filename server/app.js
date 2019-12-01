const express = require("express");
const app = express();
const http = require('http').Server(app);
const io = require('socket.io')(http);
const ArrayList = require('arraylist');

app.get('/public', function (req, res) {
    res.sendFile(__dirname + '/public/index.html');
});

app.use(express.static('public'));
var arduinoSocket;
var userSockets = new ArrayList;
io.on('connection', function (socket) {
    socket.on("isControlSocket", function (data) {
        if (data) {
            arduinoSocket = socket;
        } else {
            userSockets.add(socket);
        }
    });
    socket.on("wantedpH", function (data) {
        arduinoSocket.emit("wantedpH", data);
    });
})

http.listen(3000, function () {
    console.log('listing on port 80');
});