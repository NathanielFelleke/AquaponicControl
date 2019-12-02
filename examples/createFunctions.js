const Light = require("../models/light-model");
const User = require("../models/user-model");
const Lock = require("../models/lock-model");
const Scanner = require("../models/scanner-model");
var createLifXLight = function createLifXLight(room,scannerId,lightId,lifxId){

var newLight = new Light({
    room:room,
    scannerId: scannerId,
    lightModel: "LIFX",
    lightId: lightId,
    lifxId: lifxId
})

newLight.save(function(error,light){
    if(error){
        console.log(error);
        return
    }
    console.log("Saved" + light);

})

}

var createNewUser = function createNewUser(firstName,lastName,code,accessLevel,relationshipToAdmin,isAdmin){

User.findOne({
    isAdmin:true
}).then(function(currentUser){
    if(currentUser){
        isAdmin = false;
    }
    if(isAdmin){
        accessLevel = 10;
    }
    var newUser = new User({
        firstName: firstName,
        lastName: lastName,
        hidCode: code,
        accessLevel: accessLevel,
        relationshipToAdmin: relationshipToAdmin,
        isAdmin: isAdmin
    })
    newUser.save(function(error,user){
        console.log("Saved" + user);
    })
})

}

var createNewScanner = function createNewScanner(room,scannerId,lockId,lockedByDefualt,accessLevelNeeded,isAdminOnly,ip){
    var newScanner = new Scanner({
        room:room,
        scannerId: scannerId,
        lockId:lockId,
        accessLevelNeeded: accessLevelNeeded,
        ip: ip,
        isAdminOnly:isAdminOnly
    })
    
    newScanner.save(function(error,scanner){
        if(error){
            console.log(error);
            return
        }
        console.log("Saved" + scanner);
    
    })
    

}

var createNewLock = function createNewLock(room,lockId,lockedByDefualt,accessLevelNeeded,isAdminOnly,ip){

    var newLock = new Lock({
        room:room,
      
        lockId:lockId,
        lockedByDefualt: true,
        lockModel: "Solenoid",
        accessLevelNeeded: accessLevelNeeded,
        ip: ip,
        isAdminOnly:isAdminOnly
      
        
    })
    
    newLock.save(function(error,lock){
        if(error){
            console.log(error);
            return
        }
        console.log("Saved" + lock);
    
    })
    
}

module.exports={
    createNewScanner: createNewScanner, 
    createLifXLight: createLifXLight,
    createNewUser: createNewUser,
    createNewLock: createNewLock
}