we could make transmission more efficient by not sending timestamp for subsequent measurements with the same timestamp.
we need to use 1 bit in channelType for this, reducing number of channels to 128
this could all be in a next version of payloadencoder