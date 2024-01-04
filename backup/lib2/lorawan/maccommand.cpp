#include "maccommand.h"

const char* toString(macCommand aCommand, linkDirection aDirection) {
    if (aDirection == linkDirection::uplink) {
        switch (aCommand) {
            case macCommand::linkCheckRequest:
                return "linkCheckRequest";
                break;
            case macCommand::linkAdaptiveDataRateAnswer:
                return "linkAdaptiveDataRateAnswer";
                break;
            case macCommand::dutyCycleAnswer:
                return "dutyCycleAnswer";
                break;
            case macCommand::receiveParameterSetupAnswer:
                return "receiveParameterSetupAnswer";
                break;
            case macCommand::deviceStatusAnswer:
                return "deviceStatusAnswer";
                break;
            case macCommand::newChannelAnswer:
                return "newChannelAnswer";
                break;
            case macCommand::receiveTimingSetupAnswer:
                return "receiveTimingSetupAnswer";
                break;
            case macCommand::transmitParameterSetupAnswer:
                return "transmitParameterSetupAnswer";
                break;
            case macCommand::downlinkChannelAnswer:
                return "downlinkChannelAnswer";
                break;
            case macCommand::deviceTimeRequest:
                return "deviceTimeRequest";
                break;
            default:
                return "unknown MAC command";
        }
    } else {
        switch (aCommand) {
            case macCommand::linkCheckAnswer:
                return "linkCheckAnswer";
                break;
            case macCommand::linkAdaptiveDataRateRequest:
                return "linkAdaptiveDataRateRequest";
                break;
            case macCommand::dutyCycleRequest:
                return "dutyCycleRequest";
                break;
            case macCommand::receiveParameterSetupRequest:
                return "receiveParameterSetupRequest";
                break;
            case macCommand::deviceStatusRequest:
                return "deviceStatusRequest";
                break;
            case macCommand::newChannelRequest:
                return "newChannelRequest";
                break;
            case macCommand::receiveTimingSetupRequest:
                return "receiveTimingSetupRequest";
                break;
            case macCommand::transmitParameterSetupRequest:
                return "transmitParameterSetupRequest";
                break;
            case macCommand::downlinkChannelRequest:
                return "downlinkChannelRequest";
                break;
            case macCommand::deviceTimeAnswer:
                return "deviceTimeAnswer";
                break;
            default:
                return "unknown MAC command";
        }
    }
}