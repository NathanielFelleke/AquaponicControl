const mongoose = require('mongoose'),
Schema = mongoose.Schema;
const lockSchema = new Schema({

  room: String,
  lockId: Number,
  lockedByDefualt: Boolean,
  lockModel: String,
  ip: Number,
  isAdminOnly: Boolean

  





});
var Lock = mongoose.model('Lock', lockSchema);

// make this available to our users in our Node applications
module.exports = Lock;