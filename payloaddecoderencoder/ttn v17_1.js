function decodeUplink(input) {
    var data = {};
    data.payloadEncodingVersion = input.fPort;

    switch (data.payloadEncodingVersion) {
        case 16:
            data.error = "Payload encoding version 16 - not supported";
            break;
        case 17:
            var nmbrOfMeasurements = input.bytes[0];
            data.nmbrOfMeasurements = nmbrOfMeasurements;
            var measurementTimestamp = input.bytes[4] << 24 | input.bytes[3] << 16 | input.bytes[2] << 8 | input.bytes[1];
            data.timestamp = measurementTimestamp;
            for (var i = 0; i < nmbrOfMeasurements; i++) {
                var byteOffset = (i * 5) + 5;
                var measurementHeader = input.bytes[byteOffset];
                var measurementValue = bytesToFloat(input.bytes[byteOffset + 1], input.bytes[byteOffset + 2], input.bytes[byteOffset + 3], input.bytes[byteOffset + 4]);
                switch (measurementHeader) {
                    case 0x09:
                        data.batteryPercentCharged = measurementValue;
                        break;
                    case 0x10:
                        data.bme680temperature = measurementValue;
                        break;
                    case 0x11:
                        data.bme680RelativeHumidity = measurementValue;
                        break;
                    case 0x18:
                        data.tsl2591lux = measurementValue;
                        break;
                    default:
                }
            }
            break;
        default:
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


