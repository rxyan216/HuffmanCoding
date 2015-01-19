#include "BitInputStream.hpp"

typedef unsigned char byte;

/** Read the next bit from the bit buffer.
 *  If the bit buffer is currently empty,
 *  fill the bit buffer by reading a char from the istream first.
 *  Return the bit read as the least signficant bit of an int.
 *  Return -1 on EOF.
 *  This must be consistent with BitOutputStream::writeBit(), in terms
 *  of ordering of bits in the stream.
 */

int BitInputStream::readBit() {
    int tempInt;
    byte tempByte;
    if ( bufi == 8 ) {
        buf = in.get();
        bufi = 0;
    }
    tempByte = (byte)buf;
    tempInt = ( (tempByte >> ( 7 - bufi )) & 1 );
    bufi += 1;
    return tempInt;
}

/** If the bit buffer contains any bits, flush the bit buffer to the istream,
 *  clear the bit buffer, and set the bit buffer index to 8.
 *  Also flush the istream itself.
 */

void BitInputStream::flush() {
    if ( bufi != 8 ) {
        bufi = 8;
    }
}
