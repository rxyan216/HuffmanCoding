#include "BitOutputStream.hpp"

/** Write the least significant bit of the argument into
 *  the bit buffer, and increment the bit buffer index.
 *  Flush to the ostream first if the bit buffer is full.
 *  This must be consistent with BitInputStream::readBit().
 */

void BitOutputStream::writeBit( int bit ) {
    buf = buf + ( bit << (7 - bufi) );
    bufi += 1;
    if ( bufi == 8 ) {
        out.put( buf );
        buf = 0;
        bufi = 0;
    }
}

/** If the bit buffer contains any bits, flush the bit buffer to the ostream,
 *  clear the bit buffer, and set the bit buffer index to 0.
 *  Also flush the ostream itself.
 */

void BitOutputStream::flush() {
    if ( bufi != 0 ) {
        out.put( buf );
        buf = 0;
        bufi = 0;
    }
}
