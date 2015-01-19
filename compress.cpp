#include <iostream>
#include <fstream>
#include <vector>

#include "HCNode.hpp"
#include "HCTree.hpp"
#include "BitOutputStream.hpp"

using namespace std;

int main( int argc, char** argv ) {

    // judge whether the number of arguments is correct
    if ( argc != 3 ) {
        cout<<"You must input TWO filenames!"<<endl;
        exit(1);
    }

    // judge whether the two filenames are different
    if ( !strcmp( argv[1], argv[2] ) ) {
        cout<<"File and its compressed version should NOT have the same name!"<<endl;
        exit(2);
    }

    HCTree hctree;

    ifstream infile;
    ofstream outfile;
    BitOutputStream bitOut( outfile );

    /* open the input file */
    infile.open( argv[1], ios::in );

    if ( !infile.is_open() ) {
        cout<<"File cannot open for reading!"<<endl;
        exit(3);
    }

    vector<int> frequency(256);
    char ch;
    int charNum = 0;
    int diffNum = 0;

    /* 
     * Read one byte at a time tallying up the
     *   number of occurrences for each symbol
     */

    while ( infile.get(ch) ) {
        frequency[ (int)(byte)ch ] += 1;
        charNum += 1;
    }

    infile.close();

    /* construct the Huffman tree */
    hctree.build( frequency );

    infile.open( argv[1], ios::in );
    outfile.open( argv[2], ios::out );

    if ( !infile.is_open() ) {
        cout<<"File cannot open for reading!"<<endl;
        exit(4);
    }

    if ( !outfile.is_open() ) {
        cout<<"Compressed version cannot open for writing!"<<endl;
        exit(5);
    }

    /* output the header describing the tree to the output file */
    for ( int i = 0; i < (int)frequency.size(); i++ ) {
        if ( frequency[i] != 0 ) {
            bitOut.writeBit( 1 );
            diffNum += 1;
            if ( frequency[i] >=1 && frequency[i] <= 255 ) {
                bitOut.writeBit( 0 );
                bitOut.writeBit( 0 );
            }
            else if ( frequency[i] >= 256 && frequency[i] <= 65535 ) {
                bitOut.writeBit( 0 );
                bitOut.writeBit( 1 );
            }
            else if ( frequency[i] >= 65536 && frequency[i] <= 16777215 ) {
                bitOut.writeBit( 1 );
                bitOut.writeBit( 0 );
            }
            else {
                bitOut.writeBit( 1 );
                bitOut.writeBit( 1 );
            }
        }
        else {
            bitOut.writeBit( 0 );
        }
    }

    bitOut.flush();

    for ( int i = 0; i < (int)frequency.size(); i++ ) {
        if ( frequency[i] != 0 ) {
            if ( frequency[i] >= 1 && frequency[i] <= 255 ) {
                outfile.put( (char)(frequency[i]) );
            }
            else if ( frequency[i] >= 256 && frequency[i] <= 65535 ) {
                outfile.put( (char)(frequency[i]/256) );
                outfile.put( (char)(frequency[i]) );
            }
            else if ( frequency[i] >= 65536 && frequency[i] <= 16777215 ) {
                outfile.put( (char)(frequency[i]/65536) );
                outfile.put( (char)(frequency[i]/256) );
                outfile.put( (char)(frequency[i]) );
            }
            else {
                outfile.put( (char)(frequency[i]/16777216) );
                outfile.put( (char)(frequency[i]/65536) );
                outfile.put( (char)(frequency[i]/256) );
                outfile.put( (char)(frequency[i]) );
            }
        }
    }

    outfile.put( (char)(charNum/16777216) );
    outfile.put( (char)(charNum/65536) );
    outfile.put( (char)(charNum/256) );
    outfile.put( (char)(charNum) );

    /*
     * rewind the file to the beginning re-reading each symbol (byte)
     * output the encoding for each symbol to the output file
     */

    if ( diffNum != 1 ) {
        while ( infile.get(ch) ) {
            hctree.encode( (byte)ch, bitOut );
        }
    }

    bitOut.flush();

    infile.close();
    outfile.close();

    return 0;

}
