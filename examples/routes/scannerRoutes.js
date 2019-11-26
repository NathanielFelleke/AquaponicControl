const router = require('express').Router();
const Scanner = require("../models/scanner-model");
const User = require("../models/user-model");
const keys = require("../config/keys");
const axios = require("axios");
var bodyParser = require('body-parser');
var request = require("request");


const headers = {
  "Authorization": "Bearer c128d83ffd6364f16d4a276aa44542cd8702b388b0293796a6ba3e910d69f120"

}

router.use(bodyParser.urlencoded()); // support json encoded bodies
router.use(bodyParser.urlencoded({
  extended: true
}));

router.post('/read/:id', function (req, response) {
var whatToReturn = "0"

  console.log(req.body)
  var success = "0";
  var success2 = "0"
  console.log(req.body.hidCode)
  Scanner.findOne({
    scannerId: req.body.scannnerId
  }).then(function (currentScanner) {
    console.log("hello")
    User.findOne({
      hidCode: req.body.hidCode
    }).then(function (currentUser) {
      if (currentUser.isAdmin) {
        response.end("1");
        axios.get("https://cloud.boltiot.com/remote/" + keys.Bolt.apiKey + "/digitalWrite?pin=0&state=HIGH&deviceName=" + keys.Bolt.lockControl).then(function (req, res) {
          console.log(req.data);
          success = "" + req.data.success;

          setTimeout(function () {
            axios.get("https://cloud.boltiot.com/remote/" + keys.Bolt.apiKey + "/digitalWrite?pin=0&state=LOW&deviceName=" + keys.Bolt.lockControl).then(function (req, res) {
              success2 = "" + req.data.success
            })
          }, 1500)
        })
      } else if (currentUser.accessLevel >= currentScanner.accessLevelNeeded || currentUser.isAdmin) {
        response.end("1");
        //TO-DO write code to unlock the door with axios get request
        var request = require("request");
        console.log("" + Math.round(5 / currentUser.accesslevel))
        var options = {
          method: 'POST',
          url: 'https://api.lifx.com/v1/lights/id:d073d5206e76/effects/pulse',
          headers: {
            Authorization: 'Bearer c128d83ffd6364f16d4a276aa44542cd8702b388b0293796a6ba3e910d69f120',
            'Content-Type': 'application/x-www-form-urlencoded'
          },
          form: {
            period: "" + Math.round(5 / currentUser.accessLevel),
            cycles: '4',
            color: 'green'
          }
        };

        request(options, function (error, response, body) {
          if (error) throw new Error(error);


          setTimeout(function () {
            axios.get("https://cloud.boltiot.com/remote/" + keys.Bolt.apiKey + "/digitalWrite?pin=0&state=HIGH&deviceName=" + keys.Bolt.lockControl).then(function (req, res) {
              console.log(req.data);
              success = "" + req.data.success;

              setTimeout(function () {
                axios.get("https://cloud.boltiot.com/remote/" + keys.Bolt.apiKey + "/digitalWrite?pin=0&state=LOW&deviceName=" + keys.Bolt.lockControl).then(function (req, res) {
                  success2 = "" + req.data.success
                })
              }, 1500)

            });
          }, (10 / currentUser.accessLevel) * 2000);
        });


      }
      else{
        response.end("-1");
      }



    })



  });

})

router.post('/light/:id', function (request, response) {

  axios.post("https://api.lifx.com/v1/lights/id:d073d5206e76/toggle", {}, {
    headers: headers
  }).then(function (req, res) {
    console.log("hello")


    var options = {
      method: 'GET',
      url: 'https://api.lifx.com/v1/lights/id:d073d5206e76',
      headers: {
        'cache-control': 'no-cache',
        "Authorization": "Bearer c128d83ffd6364f16d4a276aa44542cd8702b388b0293796a6ba3e910d69f120"
      },
      form: {
        undefined: undefined
      }
    };
    var request = require("request")
    request(options, function (error, res, req) {
      var power = "";
      if (error) throw new Error(error);
      for (var i = 143; i < 146; i++) {
        power += req[i];

      }
      console.log(power)

      if (power != "off") {
        console.log("1")
        response.end("1");

      } else {
        response.end("0")
      }
    });



  });


})
router.post('/request/:id', function (request, response) {

  var request = require("request");
  
  var options = {
    method: 'POST',
    url: 'https://api.lifx.com/v1/lights/id:d073d5206e76/effects/pulse',
    headers: {
      Authorization: 'Bearer c128d83ffd6364f16d4a276aa44542cd8702b388b0293796a6ba3e910d69f120',
      'Content-Type': 'application/x-www-form-urlencoded'
    },
    form: {
      period: '2',
      cycles: '6',
      color: 'rgb:255,159,70'
    }
  };

  request(options, function (error, response, body) {
    if (error) throw new Error(error);
console.log(body)

  });
response.end("0")
})

module.exports = router;