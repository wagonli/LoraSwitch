var util = require("util");
var unirest = require("unirest");
var hexToDec = require("../tools/hexToDecimalConverter.js");
var moment = require("moment");

module.exports.status = function(properties, callback) {
    var serviceUri = util.format("%sapi/v0/data/streams/urn:lora:%s!uplink?limit=1", properties["lom.uri"], properties["device.uid"]);
    unirest.get(serviceUri)
         .headers({"X-API-KEY": properties["lom.api.key"], "Accept": "application/json"})
         .end(function (response) {
             var body = response.body[0];
             var value = body.value;
             var p = value.payload;
             var powerStatus = {};
             powerStatus.power = String.fromCharCode(hexToDec.convert(p.slice(0,2)));
             powerStatus.relay = String.fromCharCode(hexToDec.convert(p.slice(2,4)));
             powerStatus.battery = String.fromCharCode(hexToDec.convert(p.slice(4,6)), 
           hexToDec.convert(p.slice(6,8)), 
           hexToDec.convert(p.slice(8,10)), 
           hexToDec.convert(p.slice(10,12)));
             powerStatus.timestamp = moment(body.timestamp).unix();
             powerStatus.signalLevel = value.signalLevel;
             callback(powerStatus);
         });
}