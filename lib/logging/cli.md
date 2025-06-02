# Steps

1. Make Tx interrupt driven
2. Make Rx interrupt driven
3. Start unitTesting CLI : mock the receiving of a byte - is in fact nothing more than pushing a byte onto rxBuffer

* maybe circularBuffer needs an extension to pop a number of bytes, io one byte at a time..