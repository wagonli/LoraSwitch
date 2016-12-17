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
  // format("%s/api/v0/vendors/lora/devices/%s/commands", lomUri, deviceUid);
  //   }

    // @Override
    // public String switchPower(boolean switchValue) throws IOException {
    //     log.info("Received switch request with value {}", switchValue);
    //     RequestBody body = RequestBody.create(JSON, format("{\"data\": \"%s\", \"port\": 5, \"confirmed\": true}", switchValue ? "F1" : "F0"));
    //     Request request = new Request.Builder().url(commandUri)
    //                                            .header("X-API-KEY", apiKey)
    //                                            .header("Accept", "application/json")
    //                                            .post(body)
    //                                            .build();
    //     okhttp3.Response response = client.newCall(request).execute();
    //     return response.body().string();
    // }
})

app.use("/",router);

app.use("*",function(req,res){
  res.sendStatus(404);
});

app.listen(8080,function(){
  console.log("Live at Port 8080");
  console.log(properties["lom.uri"]);
});
