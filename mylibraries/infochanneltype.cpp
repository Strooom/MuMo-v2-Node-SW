// ######################################################################################
// ### MuMo node : https://github.com/Strooom/MuMo-v2-Node-SW                         ### 
// ### Author : Pascal Roobrouck - https://github.com/Strooom                         ### 
// ### License : CC 4.0 BY-NC-SA - https://creativecommons.org/licenses/by-nc-sa/4.0/ ### 
// ######################################################################################

#include "infochanneltype.h"

const char* toString(const infoChannelType aChannel)
{
	switch (aChannel)
	{
	case infoChannelType::batteryLevel:
		return "batteryLevel";
		break;
	
	default:
		return "unknown infoChannel";
		break;
	}
}