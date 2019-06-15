var data = content.split('#');
var result = {};
result.gwDate = new Date().toISOString();
if (Array.isArray(data)) {
    result.content = data[0];
    if (data[0] === 'data') {
        var type = data[1];
        if (type == '0') {
            result.type = 'DUMMY';
            result.dummy = parseFloat(data[1]);
        }
        else if (type === '1') {
            result.type = 'PLANT';
            result.light = parseFloat(data[2]);
            result.humidity = parseFloat(data[3]);
            result.temperature = parseFloat(data[4]);
            result.sm = parseFloat(data[4]);
        } else if (type === '2') {
            result.type = 'WEATHER';
            result.luminity = parseFloat(data[2]);
            result.uv = parseFloat(data[3]);
            result.ir = parseFloat(data[4]);
            result.humidity = parseFloat(data[5]);
            result.presure = parseFloat(data[6]);
            result.temperature = parseFloat(data[7]);
            result.rain = parseFloat(data[8]);
        }
    } else if (data[0] === 'status') {
        for (i = 1; i < data.length; i++) {
            let value = data[i].split(':');
            result[value[0]] = ('true' === value[1]);
        }
    }
}