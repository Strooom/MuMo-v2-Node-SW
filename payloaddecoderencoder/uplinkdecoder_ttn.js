function decodeUplink(input) {
    var data = {
        payloadEncodingVersion: 0,
        measurementGroups: []
    };

    data.payloadEncodingVersion = input.fPort;

    switch (data.payloadEncodingVersion) {
        case 16:
            delete data.measurementGroups;
            for (var i = 1; i < input.bytes.length;) {
                var measurementChannel = input.bytes[i];
                var measurementTimestamp = input.bytes[i + 4] << 24 | input.bytes[i + 3] << 16 | input.bytes[i + 2] << 8 | input.bytes[i + 1];
                var floatValue = bytesToFloat(input.bytes[i + 5], input.bytes[i + 6], input.bytes[i + 7], input.bytes[i + 8])

                switch (measurementChannel) {
                    case 0x01:
                        data.battery = floatValue;
                        break;

                    case 0x10:
                        data.temperature = floatValue;
                        break;

                    case 0x11:
                        data.humidity = floatValue;
                        break;

                    case 0x12:
                        data.pressure = floatValue;
                        break;

                    default:
                }
                i += 9;
            }
            break;

        case 17:
            var bytesConsumed = 0;
            while (bytesConsumed < input.bytes.length) {
                var measurementGroup = {
                    timestamp: 0,
                    measurements: []
                };
                var nmbrOfMeasurements = input.bytes[bytesConsumed + 0];
                measurementGroup.timestamp = input.bytes[bytesConsumed + 4] << 24 | input.bytes[bytesConsumed + 3] << 16 | input.bytes[bytesConsumed + 2] << 8 | input.bytes[bytesConsumed + 1];
                for (var i = 0; i < nmbrOfMeasurements; i++) {
                    var measurement = {
                        deviceIndex: 0,
                        channelIndex: 0,
                        value: 0
                    };
                    var byteOffset = (i * 5) + 5;
                    measurement.deviceIndex = (input.bytes[bytesConsumed + byteOffset] & 0xF8) >> 3;
                    measurement.channelIndex = (input.bytes[bytesConsumed + byteOffset] & 0x07);
                    measurement.value = bytesToFloat(input.bytes[bytesConsumed + byteOffset + 1], input.bytes[bytesConsumed + byteOffset + 2], input.bytes[bytesConsumed + byteOffset + 3], input.bytes[bytesConsumed + byteOffset + 4]);
                    measurementGroup.measurements.push(measurement);
                }
                data.measurementGroups.push(measurementGroup);
                bytesConsumed += (nmbrOfMeasurements * 5) + 5;
            }
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


// TEST-vectors
// port 16 : 02013F2AD2641B2F5540103F2AD26444DFB341113F2AD26471012342123F2AD26482E87D44
// port 17 : 04452E606608F6284440090000803F10B6ECB041181F854B4002452E606608F6284440090000803F