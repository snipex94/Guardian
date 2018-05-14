var express = require('express');
var router = express.Router();
var sqlite3 = require('sqlite3').verbose();
let plainTextParser = require('plainTextParser');

var db = new sqlite3.Database('dev.sqlite');

var alarmCache = {};

const names = {1: "Johnny Blaze", 2: "Gasilec Samo", 3: "Firestorm", 4:"Robbie Reyes", 5: "Danny Ketch", 6: "Abby Brand", 50: "Muca Copatarica"};



/* GET home page. */
router.get('/', function (req, res, next) {
    res.send('public/index.html');
});

router.post('/pushSimulator', function (req, res, next) {
    insertNewValues(req.body, function (err) {
        if (!err) {
            res.send({success: true});
        } else {
            res.send({success: false, msg: err});
        }
    });
});


router.post('/push', function (req, res, next) {
    console.log(Object.keys(req.body)[0]);
    var splitData= Object.keys(req.body)[0].split(':');
    var data= {
        device_id: parseInt(splitData[0]),
        temp: parseFloat(splitData[1]),
        movement: parseInt(splitData[2]),
        timestamp: new Date().getTime()
    };

    console.log(data);

    insertNewValues(data, function (err) {
        if (!err) {
            res.send({success: true});
        } else {
            res.send({success: false, msg: err});
        }
    })
});

router.get('/getDevices', function (req, res, next) {
    db.all(`SELECT device_id from data GROUP BY device_id`, function (err, data_res) {
        if (!err) {
            res.status(200).send({success: true, data: data_res});
        } else {
            res.status(300).send({success: false, msg: err});
        }
    })
});

router.get('/lastUpdate', function (req, res, next) {
    const query = `select t.device_id, t.temp, t.movement, t.timestamp from data t
INNER JOIN (select device_id, max(timestamp ) as maxdate from data GROUP BY device_id)
tm on t.device_id = tm.device_id and t.timestamp = tm.maxdate ORDER BY t.device_id`;

    db.all(query, function (err, data) {
        if (!err) {
            for(let i = 0; i < data.length; i++) {
                data[i]['name'] = names[data[i].device_id];
            }
            res.send({success: true, data: data});

        } else {
            res.send({success: false, msg: err})
        }
    })
});

router.get('/alarm', function (req, res, next) {
    const ID = parseInt(req.query.device_id)

    if(!alarmCache.hasOwnProperty(ID)) {
        alarmCache[ID] = {};
    }
    console.log('Trigger alarm for: ' + ID)
    alarmCache[ID]['start'] = new Date();
    res.send({success:true});
})

//temp, gibanje, prozi alarm na 1/vseh, indikator alarma

function insertNewValues(_data, _callback) {

    db.run(`INSERT INTO data(timestamp,temp,movement,device_id) VALUES(?,?,?,?)`,
        [_data.timestamp, _data.temp, _data.movement, _data.device_id], function (err) {
            if (!err) {
                console.log("Successfully inserted");
                return _callback(null);
            } else {
                console.error(err);

                return _callback(err);
            }
        });
}


module.exports = router;
