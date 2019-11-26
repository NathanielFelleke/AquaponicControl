const mongoose = require('mongoose'),
Schema = mongoose.Schema;
const scannerSchema = new Schema({

  room: String,
  accessLevelNeeded: Number,
  scannerId: Number,
  lockId: Number,
  isAdminOnly: Boolean,
  ip: String
  





});
var Scanner = mongoose.model('Scanner', scannerSchema);

// make this available to our users in our Node applications
module.exports = Scanner;