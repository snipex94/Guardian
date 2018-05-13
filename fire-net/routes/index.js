var express = require('express');
var router = express.Router();
var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('dev.sqlite');

/* GET home page. */
router.get('/', function (req, res, next) {
    res.send('public/index.html');
});

router.post('/push', function (req, res, next) {
    insertNewValues(req.body, function (err) {
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
            res.send({success: true, data: data_res});
        } else {
            res.send({success: false, msg: err});
        }
    })
})

function insertNewValues(_json, _callback) {
    var data = [new Date().valueOf(), parseFloat(_json.value), _json.sensor_type, _json.device_id];
    console.log(data);
    db.run(`INSERT INTO data(timestamp,value,type,device_id) values (?,?,?,?)`, data, function (err) {
        _callback(err);
    })
}



module.exports = router;
