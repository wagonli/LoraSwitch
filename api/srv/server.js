var express = require("express");
require("pkginfo")(module, "version");
var properties = require("../config/properties.json");
var unirest = require("unirest");
var util = require("util");
var cors = require("cors");
var moment = require("moment");
var app = express();
var router = express.Router();
var logger = require("winston");
var switchService = require("./service/switchService.js")

const SERVER_PORT = 5555;

var convert = function(numberInHex) {
    return parseInt(numberInHex, 16).toString(10);
};

app.use(cors());

router.use(function (req,res,next) {
    logger.log("info", "%s;%s;%s;%s", req.method, req.url, req.ip, req.rawHeaders);
    next();
});

router.get("/version",function(req,res){
    res.send(module.exports.version);
});

router.put("/switch/:switchValue", function(req,res) {
    switchService.switch(properties, req.params["switchValue"], function(response) {
        res.send(response.body);
    });
});

router.get("/power/status", function(req, res) {
    var serviceUri = util.format("%sapi/v0/data/streams/urn:lora:%s!uplink?limit=1", properties["lom.uri"], properties["device.uid"]);
    unirest.get(serviceUri)
         .headers({"X-API-KEY": properties["lom.api.key"], "Accept": "application/json"})
         .end(function (response) {
             var body = response.body[0];
             var value = body.value;
             var p = value.payload;
             var powerStatus = {};
             powerStatus.power = String.fromCharCode(convert(p.slice(0,2)));
             powerStatus.relay = String.fromCharCode(convert(p.slice(2,4)));
             powerStatus.battery = String.fromCharCode(convert(p.slice(4,6)), 
           convert(p.slice(6,8)), 
           convert(p.slice(8,10)), 
           convert(p.slice(10,12)));
             powerStatus.timestamp = moment(body.timestamp).unix();
             powerStatus.signalLevel = value.signalLevel;
             res.json(powerStatus);
         });
});

app.use("/",router);

app.use("*",function(req,res){
    res.sendStatus(404);
});

app.listen(SERVER_PORT,function(){
    logger.log("info" , "Hello, i'm live at Port %s", SERVER_PORT);
    logger.log("info", properties["lom.uri"]);
});
