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