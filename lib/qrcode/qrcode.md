# qrCode

## web resources explaining how to construct a qrCode

* https://www.thonky.com/qr-code-tutorial/
* https://dev.to/maxart2501/series/13444
* https://www.qrcode-tiger.com/qr-code-error-correction
* https://amodm.com/blog/2024/05/28/qr-codes-a-visual-explainer
* https://www.keyence.eu/ss/products/auto_id/codereader/basic_2d/qr.jsp
* https://www.qrcode.com/en/about/version.html

# vocabulary
* module : 1 pixel/square of the qrCode, can be white (0) or black (1)
* structural : modules to allow decoding, not containing user data 
* informational : modules containing user data
* bitVector<size> data; this holds the to be encoded data, extended with error-correction
* bitMatrix<size> modules; this is a 2-dimensional array, holding the pixels (modules) of the qrCode

# how to use this library
This library is developed for use in embedded software where it is recommended to avoid allocating dynamic memory.
qrCodes come in different sizes, called 'version' (from 1 to 40), and the larger the version, the more memory is needed.
In order to not waste memory, you define at compile-time what will be the maximum version (size) you may need, and memory for this maximum is allocated statically.

In case you know at compile-time what version you need, it's as simple as calling qrCode::encodeData();
In case you only know at run-time what will be the size of the payload, you can call qrCode::versionNeeded(); set this version and then call encodeData().


# Coordinates
```top-left is (0,0)
x runs horizontal left to right
y runs vertical top to bottom
0,0 ..... x,0
.           .
.           .
.           .
.           .
.           .
0,y ..... x,y
```

# Library Organisation

1. public API functions
    * versionNeeded : returns the minimum version needed to accommodate the payload at the given errorCorrection
    * errorCorrectionPossible : returns the maximum errorCorrection possible given the payload and the version
    * generateQrCode : takes payload, version and errorCorrection level and builds the qrCode matrix
    * getModule(x,y) : allows to read back the result, pixel by pixel

2. encoding user payload
    * determine what encodingType is needed
    * encode cstring into bitVector

3. error correction
    * append errorCorrection codewords after payload
    * interleave : interleave data and errorCorrection when multiple blocks are being used

4. drawing
    * drawing function patterns
    * drawing user data
    * applying mask



Following functions are needed for qrCode::versionNeeded() and take a candidate version as parameter.
* versionNeeded
* payloadLengthInBits
* nmbrOfDataModules
* characterCountIndicatorLength
* nmbrOfTotalModules
* nmbrOfFunctionModules
* size
* nmbrOfAlignmentPatterns
* mbrOfAlignmentPatternRowsOrCols

All other functions use the class members theVersion, theEncodingFormat and theErrorCorrectionLevel and thus don't need passing them as parameters.