function Decoder(payload, port) {
    var decoded = [];
    switch (port) {
        case 17:
            var bytesConsumed = 0;
            while (bytesConsumed < payload.length) {
                var nmbrOfMeasurements = payload[bytesConsumed + 0];
                timestamp = payload[bytesConsumed + 4] << 24 | payload[bytesConsumed + 3] << 16 | payload[bytesConsumed + 2] << 8 | payload[bytesConsumed + 1];
                for (var i = 0; i < nmbrOfMeasurements; i++) {
                    var byteOffset = (i * 5) + 5;
                    var deviceIndex = (payload[bytesConsumed + byteOffset] & 0xF8) >> 3;
                    var channelIndex = (payload[bytesConsumed + byteOffset] & 0x07);
                    var value = bytesToFloat(payload[bytesConsumed + byteOffset + 1], payload[bytesConsumed + byteOffset + 2], payload[bytesConsumed + byteOffset + 3], payload[bytesConsumed + byteOffset + 4]);
                    decoded.push({
                        "field": getDeviceAndChannelName(deviceIndex, channelIndex),
                        "value": value,
                        "timestamp": timestamp
                    });
                }
                bytesConsumed += (nmbrOfMeasurements * 5) + 5;
            }
            break;
        default:
    }
    return decoded;
}


function getDeviceAndChannelName(deviceIndex, channelIndex) {
    switch (deviceIndex) {
        case 0:
            return "MCU";
        case 1:
            switch (channelIndex) {
                case 0:
                    return "BATTERYVOLTAGE";
                case 1:
                    return "BATTERYPERCENTCHARGED";
                default:
                    return "UNKNOWNCHANNEL";
            }
            break;
        case 2:
            switch (channelIndex) {
                case 0:
                    return "BME680TEMPERATURE";
                case 1:
                    return "BME680RELATIVEHUMIDITY";
                case 2:
                    return "BME680BAROMETRICPRESSURE";
                default:
                    return "UNKNOWNCHANNEL";
            }
            break;
        case 3:
            switch (channelIndex) {
                case 0:
                    return "TSL2591VISIBLELIGHT";
                default:
                    return "UNKNOWNCHANNEL";
            }
            break;
        default:
            return "UNKNOWNDEVICE";
    }
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
