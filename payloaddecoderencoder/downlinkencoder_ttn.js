var command = ["setSensorDeviceChannel", "retransmitMeasurements"];

function encodeDownlink(input) {
    switch (input.data.command) {
        case "setSensorDeviceChannel":
            return {
                fPort: 16 + command.indexOf(input.data.command),
                bytes: [deviceIndex, channelIndex, oversampling, prescaling % 256, prescaling >> 8],
            };

        case "retransmitMeasurements":
            return {
                fPort: 16 + command.indexOf(input.data.command),
                bytes: [uplinkFramecount, channelIndex, oversampling, prescaling, prescaling],
            };

        default:
            return {
                errors: ["unknown command"],
            };
    }
}

// function decodeDownlink(input) {
//     switch (input.fPort) {
//         case 4:
//             return {
//                 data: {
//                     color: colors[input.bytes[0]],
//                 },
//             };
//         default:
//             return {
//                 errors: ["unknown FPort"],
//             };
//     }
// }