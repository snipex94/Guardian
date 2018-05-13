var express = require('express');
var router = express.Router();
var sqlite3 = require('sqlite3').verbose();
let plainTextParser = require('plainTextParser');

var db = new sqlite3.Database('dev.sqlite');

var cache = {};


/* GET home page. */
router.get('/', function (req, res, next) {
    res.send('public/index.html');
});

router.post('/push', plainTextParser, function (req, res, next) {
    console.log(req.body);
    insertNewValues(Object.keys(req.body)[0], function (err) {
        if (!err) {
            res.send({success: true});
        } else {
            res.send({success: false, msg: err});
        }
    });
});

router.get('/getDevices', function (req, res, next) {
    db.all(`SELECT * from data GROUP BY device_id`, function (err, data_res) {
        if (!err) {
            res.status(200).send({success: true, data: data_res});
        } else {
            res.status(300).send({success: false, msg: err});
        }
    })
});

router.get('/lastUpdate', function (req, res, next) {
    var lasts = {};
    for(var key in cache) {
        var item = {client: key, timestamp: cache[key][cache[key].length-1].time}
        lasts[key] = item;
    }
    res.send({success:true, data: lasts});
});

function insertNewValues(text, _callback) {

    var data = text.split(':');
    const clientId = data[0];
    const value = parseFloat(data[1]);

    if(!cache[clientId]) {
        cache[clientId] = [];
    }

    console.log({time: new Date().valueOf(), value: value});
    cache[clientId].push({time: new Date().valueOf(), value: value});
    _callback(null);
}

//temp, gibanje, prozi alarm na 1/vseh, indikator alarma



module.exports = router;
