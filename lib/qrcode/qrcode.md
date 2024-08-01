https://www.thonky.com/qr-code-tutorial/
https://dev.to/maxart2501/series/13444
https://www.qrcode-tiger.com/qr-code-error-correction
https://amodm.com/blog/2024/05/28/qr-codes-a-visual-explainer


vocabulary :
* module : 1 pixel/square of the qrCode, can be white (0) or black (1)
* structural : modules to allow decoding, not containing user data 
* informational : modules containing user data








* isFunction is a bitArray with '1' if the module is a function and '0' if it is user data

Uses two bitArrays : 
* one with the resulting data of the QR code : called modules
* one with 1's for every non-data pixel : isFunction : so no data will be written in locations where these functions are needed
