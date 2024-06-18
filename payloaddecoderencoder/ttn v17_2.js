// This is a variant returning an array-object, more aligned to the object model in the device
// test-vector from device : 04452E606608F6284440090000803F10B6ECB041181F854B40
// test-vector from device : 04452E606608F6284440090000803F10B6ECB041181F854B4002452E606608F6284440090000803F




function decodeUplink(input) {
    var data = {
        payloadEncodingVersion: 0,
        measurementGroups: []
    };

    data.payloadEncodingVersion = input.fPort;

    switch (data.payloadEncodingVersion) {
        case 17:
            var measurementGroup = {
                timestamp: 0,
                measurements: []
            };
            var nmbrOfMeasurements = input.bytes[0];
            measurementGroup.timestamp = input.bytes[4] << 24 | input.bytes[3] << 16 | input.bytes[2] << 8 | input.bytes[1];
            for (var i = 0; i < nmbrOfMeasurements; i++) {
                var measurement = {
                    deviceIndex: 0,
                    channelIndex: 0,
                    value: 0
                };
                var byteOffset = (i * 5) + 5;
                measurement.deviceIndex = (input.bytes[byteOffset] & 0xF8) >> 3;
                measurement.channelIndex = (input.bytes[byteOffset] & 0x07);
                measurement.value = bytesToFloat(input.bytes[byteOffset + 1], input.bytes[byteOffset + 2], input.bytes[byteOffset + 3], input.bytes[byteOffset + 4]);
                measurementGroup.measurements.push(measurement);
            }
            data.measurementGroups.push(measurementGroup);
            break;
        default:
            data.error = "Payload encoding version not supported";
    }

    return {
        data: data
    };
}

function bytesToFloat(byte1, byte2, byte3, byte4) {
    var bits = byte4 << 24 | byte3 << 16 | byte2 << 8 | byte1;
    var sign = (bits >>> 31 === 0) ? 1.0 : -1.0;
    var e = bits >>> 23 & 0xff;
    var m = (e === 0) ? (bits & 0x7fffff) << 1 : (bits & 0x7fffff) | 0x800000;
    var f = sign * m * Math.pow(2, e - 150);
    return f;
}


