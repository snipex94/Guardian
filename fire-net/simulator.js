var request = require('request');

const prefix = "FIRE-";

setInterval(function () {
        for (var id = 1; id < 4; id++) {
            // var id = Math.floor(Math.random() * 3 + 1);
            var options = {
                uri: 'http://localhost:16179/pushSimulator',
                method: 'POST',
                json: {
                    timestamp: new Date().getTime(),
                    device_id: id,
                    movement: Math.floor(Math.random() * 2),
                    temp: (Math.random() * 3 + 25 + id)
                }
            };
            request.post(
                options,
                function (error, response, body) {
                    if (!error) {
                        console.log(body)
                    } else {
                        console.error(error)
                    }
                });
        }
    },
    2000
);