#include <iostream>
#include <fstream>
#include <vector>

#include "HCNode.hpp"
#include "HCTree.hpp"
#include "BitInputStream.hpp"

using namespace std;

int main( int argc, char** argv ) {

    // judge whether the number of arguments is correct
    if ( argc != 3 ) {
        cout<<"You must input TWO filenames!"<<endl;
        exit(1);
    }

    // judge whether the two filenames are different
    if ( !strcmp( argv[1], argv[2] ) ) {
        cout<<"File and its uncompressed version should NOT have the same name!"<<endl;
        exit(2);
    }

    HCTree hctree;

    ifstream infile;
    ofstream outfile;
    BitInputStream bitIn( infile );

    /* open the input file */
    infile.open( argv[1], ios::in );

    if ( !infile.is_open() ) {
        cout<<"File cannot open for reading!"<<endl;
        exit(3);
    }

    char ch, ch_1, ch_2, ch_3, ch_4;
    int charNum = 0;
    int diffNum = 0;
    int diffNumSymbol = 0;
    int temp, temp_1, temp_2;

    vector<int> frequency(256);
    vector<int> ifExist(256);
    vector<int> countLength(256);

    /* use the header to reconstruct the Huffman tree */
    for ( int i = 0; i < (int)frequency.size(); i++ ) {
        temp = bitIn.readBit();
        if ( temp != 0 ) {
            ifExist[i] = 1;
            diffNum += 1;
            temp_1 = bitIn.readBit();
            temp_2 = bitIn.readBit();
            if ( temp_1 == 0 && temp_2 == 0 ) {
                countLength[i] = 1;
            }
            else if ( temp_1 == 0 && temp_2 == 1 ) {
                countLength[i] = 2;
            }
            else if ( temp_1 == 1 && temp_2 == 0 ) {
                countLength[i] = 3;
            }
            else if ( temp_1 == 1 && temp_2 == 1 ) {
                countLength[i] = 4;
            }
            else {
                cout<<"Unknown error!"<<endl;
                exit(4);
            }
        }
        else {
            ifExist[i] = 0;
            countLength[i] = 0;
            frequency[i] = 0;
        }
    }

    bitIn.flush();

    for ( int i = 0; i < (int)frequency.size(); i++ ) {
        if ( ifExist[i] != 0 ) {
            if ( countLength[i] == 1 ) {
                ch_4 = infile.get();
                frequency[i] = ((int)(unsigned char)ch_4);
            }
            else if ( countLength[i] == 2 ) {
                ch_3 = infile.get();
                ch_4 = infile.get();
                frequency[i] = ((int)(unsigned char)ch_3) * 256 + ((int)(unsigned char)ch_4);
            }
            else if ( countLength[i] == 3 ) {
                ch_2 = infile.get();
                ch_3 = infile.get();
                ch_4 = infile.get();
                frequency[i] = ((int)(unsigned char)ch_2) * 65536 + ((int)(unsigned char)ch_3) * 256 + ((int)(unsigned char)ch_4);
            }
            else if ( countLength[i] == 4 ) {
                ch_1 = infile.get();
                ch_2 = infile.get();
                ch_3 = infile.get();
                ch_4 = infile.get();
                frequency[i] = ((int)(unsigned char)ch_1) * 16777216 + ((int)(unsigned char)ch_2) * 65536 + ((int)(unsigned char)ch_3) * 256 + ((int)(unsigned char)ch_4);
            }
            else {
                cout<<"Unknown error!"<<endl;
                exit(5);
            }
        }
    }

    hctree.build( frequency );

    ch_1 = infile.get();
    ch_2 = infile.get();
    ch_3 = infile.get();
    ch_4 = infile.get();

    charNum = ((int)(unsigned char)ch_1) * 16777216 + ((int)(unsigned char)ch_2) * 65536 + ((int)(unsigned char)ch_3) * 256 + ((int)(unsigned char)ch_4);

    outfile.open ( argv[2], ios::out );

    if ( !outfile.is_open() ) {
        cout<<"Uncompressed version cannot open for writing!"<<endl;
        exit(6);
    }

    /*
     * read all the remaining bits, one at a time, to find the original value
     * write the symbol to the output file
     */

    if ( !hctree.rootEmpty() ) {
        if ( diffNum != 1 ) {
            while ( charNum ) {
                ch = (char)(hctree.decode( bitIn ));
                outfile.put( ch );
                charNum--;
            }
        }
        else {
            for ( int i = 0; i < (int)frequency.size(); i ++ )
                if ( frequency[i] != 0 ) {
                    diffNumSymbol = i;
                    break;
                }
            while ( charNum ) {
                ch = (char)(diffNumSymbol);
                outfile.put( ch );
                charNum--;
            }
        }
    }

    infile.close();
    outfile.close();

    return 0;

}
