var express = require('express');
var pkginfo = require('pkginfo')(module, 'version');
var properties = require('./properties.json');
var unirest = require('unirest');
var util = require('util');
var cors = require('cors');
var moment = require('moment');

var app = express();
var router = express.Router();
var path = __dirname + '/views/';

var convert = function(numberInHex) {
  return parseInt(numberInHex, 16).toString(10);
}

app.use(cors());

router.use(function (req,res,next) {
  console.log("/" + req.method);
  next();
});

router.get("/version",function(req,res){
  res.send(module.exports.version);
});

router.put("/switch/:switchValue", function(req,res) {
  var serviceUri = util.format("%sapi/v0/vendors/lora/devices/%s/commands", properties["lom.uri"], properties["device.uid"]);
  unirest.post(serviceUri)
         .headers({'X-API-KEY': properties['lom.api.key'], 'Accept': 'application/json'})
         .type('json')
         .send({'data': (req.params["switchValue"] == 1 ? "F1" : "F0"), 'port': 5, 'confirmed': true})
         .end(function (response) {
           res.send(response.body);
         });
});

router.get("/power/status", function(req, res) {
  var serviceUri = util.format("%sapi/v0/data/streams/urn:lora:%s!uplink?limit=1", properties["lom.uri"], properties["device.uid"]);
  unirest.get(serviceUri)
         .headers({'X-API-KEY': properties['lom.api.key'], 'Accept': 'application/json'})
         .end(function (response) {
           var body = response.body[0];
           var value = body.value;
           var p = value.payload;
           var response = {};
           response.power = String.fromCharCode(convert(p.slice(0,2)));
           response.relay = String.fromCharCode(convert(p.slice(2,4)));
           response.battery = String.fromCharCode(convert(p.slice(4,6)), 
           convert(p.slice(6,8)), 
           convert(p.slice(8,10)), 
           convert(p.slice(10,12)));
           response.timestamp = moment(body.timestamp).unix();
           response.signalLevel = value.signalLevel;
           res.json(response);
         });
});

app.use("/",router);

app.use("*",function(req,res){
  res.sendStatus(404);
});

app.listen(5555,function(){
  console.log("Live at Port 5555");
  console.log(properties["lom.uri"]);
});
