var express = require('express');
var pkginfo = require('pkginfo')(module, 'version');
var properties = require('./properties.json');
var unirest = require('unirest');
var util = require('util');

var app = express();
var router = express.Router();
var path = __dirname + '/views/';

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
           res.send(Uint8Array.from(response.body[0].value.payload));
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
