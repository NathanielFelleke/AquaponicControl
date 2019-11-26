const mongoose = require('mongoose'),
  Schema = mongoose.Schema;
const userSchema = new Schema({

  firstName: String,
  lastName: String,
  hidCode: String,
  accessLevel: Number,
  relationshipToAdmin: String,
  isAdmin: Boolean,
  





});
var User = mongoose.model('User', userSchema);

// make this available to our users in our Node applications
module.exports = User;