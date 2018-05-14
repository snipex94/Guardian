var request = require('request');

const prefix = "FIRE-";

setInterval(function () {
    var options = {
        uri: 'http://localhost:3000/pushSimulator',
        method: 'POST',
        json: {
            timestamp: new Date().getTime(),
            device_id: Math.floor(Math.random() * 6 + 1),
            movement: Math.random() * 10,
            temp: Math.random() * 30 + 20
        },
    };
    request.post(
        options,

        function (error, response, body) {
            if(!error) {
                console.log(body)
            } else {
                console.error(error)
            }
        });
}, 1000);