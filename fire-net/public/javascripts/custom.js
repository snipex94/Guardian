var isrendered = [];

function time(t) {
    var d = new Date(t),
        h = (d.getHours() < 10 ? '0' : '') + d.getHours(),
        m = (d.getMinutes() < 10 ? '0' : '') + d.getMinutes(),
        s = (d.getSeconds() < 10 ? '0' : '') + d.getSeconds();
    return h + ':' + m + ":" + s;
}

function triggerAlarm(id) {
    console.log('Alarm form: ' + id);
    $.ajax({
        type: 'get',
        url: '/alarm',
        data: {
            device_id: id

        },
        success: function (data) {
            console.log('Triggered alarm')
        }
    });


}

function isEmpty(el) {
    return !$.trim(el.html())
}

function renderData(data) {
    $('#main_table').empty();
    var appendTabs = isEmpty($('#graph_tabs')) ? true : false;


    for (var i = 0; i < data.length; i++) {
        var row = data[i];
        // console.log(row);
        // console.log(new Date(row.timestamp))

        var ok_colour = 'green';
        var is_ok = 'fa fa-thumbs-up';
        if (row.temp > 45.0 || row.movement > 0) {
            is_ok = 'fa fa-thumbs-down';
            ok_colour = 'red'
        }

        var conn_colour = 'green';
        var is_connected = 'fa fa-wifi';
        if (row.timestamp < (new Date().getTime() - 5000)) {
            is_connected = 'fa fa-times';
            conn_colour = 'red';
        }

        var html = '<tr>' +
            '                    <th scope="row">' + row.device_id + '</th>' +
            '                    <td><a>' + row.name + '</a></td>' +
            '                    <td>' + time(row.timestamp) + '</td>' +
            '                    <td><i class="' + is_ok + '" aria-hidden="true" style="color: ' + ok_colour + '"></i></td>' +
            '                    <td><i class="' + is_connected + '" aria-hidden="true" style="color: ' + conn_colour + '"></i></td>' +
            '                    <td>' + Math.round(row.temp * 10) / 10 + ' °C</td>' +
            '                    <td><button type="button" class="btn btn-danger" id="alarmBtn_' + row.device_id + '" onclick="triggerAlarm(' + row.device_id + ')">Trigger</button></td>' +
            '                </tr>';
        $('#main_table').append(html);

        if (appendTabs) {
            var tabHtml =
                '                    <button id="graph_btn_' + row.device_id + '" type="button" class="btn btn-danger" style="margin-right: 5px" onclick="renderGraph('
                + row.device_id + ', \'' + row.name + '\')">' + row.name + '</button>'

            $('#graph_tabs').append(tabHtml);
        }
    }
}

function renderFunc(id, data, name) {
    var colorNames = Object.keys(window.chartColors);
    var temps = [];
    var time = [];
    for (var i = 0; i < data.length; i++) {
        temps.push(data[i].temp);
        time.push(data[i].timestamp);
    }

    var colorName = colorNames[config.data.datasets.length % colorNames.length];
    var newColor = window.chartColors[colorName];
    var newDataset = {
        label: name,

        backgroundColor: newColor,
        borderColor: newColor,
        data: temps,
        fill: false,
        id: id
    };

    config.data.datasets.push(newDataset);

    config.data.labels = time;
    window.myLine.update();


}

function removeDataset(id, name) {
    var idx = config.data.datasets.findIndex(function (el) {
        return el.id == id;
    });

    config.data.datasets.splice(idx, 1);
    window.myLine.update();
}

function renderGraph(id, name) {
    if (isrendered.indexOf(id) > -1) {
        $('#graph_btn_' + id).removeClass('btn-primary').addClass('btn-danger');
        isrendered.splice(isrendered.indexOf(id), 1);
        removeDataset(id, name);
    } else {
        $('#graph_btn_' + id).removeClass('btn-danger').addClass('btn-primary');
        $.ajax({
            type: 'get',
            url: '/getHist',
            data: {
                device_id: id

            },
            success: function (data) {
                isrendered.push(id);
                renderFunc(id, data.data, name)
            }
        });
    }


}

$('document').ready(function () {
    setInterval(function () {
        $.get("/lastUpdate", function (data, status) {
            renderData(data.data)
        })
    }, 1000);
});