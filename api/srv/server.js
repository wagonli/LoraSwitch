var express = require("express");
require("pkginfo")(module, "version");
var properties = require("../config/properties.json");
var cors = require("cors");
var app = express();
var router = express.Router();
var logger = require("winston");
var switchService = require("./service/switchService.js")
var statusService = require("./service/statusService.js")

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
        res.send(response);
    });
});

router.get("/power/status", function(req, res) {
    statusService.status(properties, function(powerStatus) {
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
