const mongoose = require('mongoose'),
Schema = mongoose.Schema;
const lightSchema = new Schema({

  room: String,
  scannerId: Number,
  LightModel: String,
  lightId: Number,
  lifxId: String

  





});
var Light = mongoose.model('Light', lightSchema);

// make this available to our users in our Node applications
module.exports = Light;