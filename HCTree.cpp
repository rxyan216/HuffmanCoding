#include <iostream>
#include <fstream>
#include <queue>
#include <vector>

#include "HCNode.hpp"
#include "HCTree.hpp"

using namespace std;

HCTree::~HCTree() {
    queue<HCNode*> q_del;
    HCNode* temp_del;

    for ( int i = 0; i < (int)leaves.size(); i++ ) {
        if ( leaves[i]->count == 0 ) {
            temp_del = leaves[i];
            delete temp_del;
        }
    }

    /* delete elements using a stack */
    if ( root != nullptr ) {
        q_del.push( root );
        while ( (int)q_del.size() != 0 ) {
            if ( q_del.front()->c0 != nullptr ) {
                q_del.push( q_del.front()->c0 );
            }
            if ( q_del.front()->c1 != nullptr ) {
                q_del.push( q_del.front()->c1 );
            }
            temp_del = q_del.front();
            q_del.pop();
            delete temp_del;
            temp_del = nullptr;
        }
    }
    temp_del = nullptr;
}

/** Use the Huffman algorithm to build a Huffman coding trie.
 *  PRECONDITION: freqs is a vector of ints, such that freqs[i] is
 *  the frequency of occurrence of byte i in the message.
 *  POSTCONDITION:  root points to the root of the trie,
 *  and leaves[i] points to the leaf node containing byte i.
 */

void HCTree::build( const vector<int>& freqs ) {
    priority_queue< HCNode*, vector<HCNode*>, HCNodePtrComp > pq;
    HCNode* temp;

    for ( int i = 0; i < (int)freqs.size(); i++ ) {
        leaves[i] = new HCNode( freqs[i], (byte)i );
    }

    for ( int i = 0; i < (int)freqs.size(); i++ ) {
        if ( leaves[i]->count != 0 )
            pq.push(leaves[i]);
    }

    if ( (int)(pq.size()) != 0 ) {
        while ( (int)(pq.size()) > 1 ) {
            pq.top()->p = new HCNode( pq.top()->count, pq.top()->symbol );
            pq.top()->p->c0 = pq.top();
            temp = pq.top()->p;
            pq.pop();
            temp->c1 = pq.top();
            pq.top()->p = temp;
            temp->count = temp->count + pq.top()->count;
            pq.pop();
            pq.push( temp );
        }

        root = pq.top();
        pq.pop();
    }

}

/** Write to the given BitOutputStream
 *  the sequence of bits coding the given symbol.
 *  PRECONDITION: build() has been called, to create the coding
 *  tree, and initialize root pointer and leaves vector.
 */

void HCTree::encode( byte symbol, BitOutputStream& out ) const {
    HCNode* temp;
    vector<int> v;
    temp = leaves[(int)symbol];

    if ( temp == root ) {
        out.writeBit( 0 );
    }
    else {
        while ( temp->p != nullptr ) {
            if ( temp->p->c0 == temp ) {
                v.push_back( 0 );
            }
            else {
                v.push_back( 1 );
            }
            temp = temp->p;
        }
        for ( int i = ((int)(v.size())) - 1; i >= 0; i-- ) {
            out.writeBit( v[i] );
        }
    }
}

/** Write to the given ofstream
 *  the sequence of bits (as ASCII) coding the given symbol.
 *  PRECONDITION: build() has been called, to create the coding
 *  tree, and initialize root pointer and leaves vector.
 *  THIS METHOD IS USEFUL FOR THE CHECKPOINT BUT SHOULD NOT
 *  BE USED IN THE FINAL SUBMISSION.
 */

void HCTree::encode( byte symbol, ofstream& out ) const {
    HCNode* temp;
    vector<char> v; 
    temp = leaves[(int)symbol];

    if ( temp == root ) {
        out.put( '0' );
    }
    else {
        while ( temp->p != nullptr ) {
            if ( temp->p->c0 == temp ) {
                v.push_back( '0' );
            }
            else {
                v.push_back( '1' );
            }
            temp = temp->p;
        }
        for ( int i = ((int)(v.size())) - 1; i >= 0; i-- ) {
            out.put( v[i] );
        }
    }
}

/** Judge whether the heap root is empty.
 *  If the heap is empty, the original file is also empty.
 *  Thus, the uncompressed file we build is empty.
 */

bool HCTree::rootEmpty() {
    if ( root == nullptr) {
        return 1;
    }
    else {
        return 0;
    }
}

/** Return symbol coded in the next sequence of bits from the stream.
 *  PRECONDITION: build() has been called, to create the coding
 *  tree, and initialize root pointer and leaves vector.
 */

int HCTree::decode( BitInputStream& in ) const {
    if ( root->c0 == nullptr && root->c1 == nullptr ) {
        if ( in.readBit() == 0 ) {
            return (int)(root->symbol);
        }
        else {
            cout<<"Wrong compressed file!"<<endl;
            return 0;
        }
    }
    else {
        HCNode* temp = root;
        while ( temp->c0 != nullptr && temp->c1 != nullptr ) {
            if ( in.readBit() == 0 ) {
                temp = temp->c0;
            }
            else {
                temp = temp->c1;
            }
        }
        return (int)(temp->symbol);
    }
}

/** Return the symbol coded in the next sequence of bits (represented as
 *  ASCII text) from the ifstream.
 *  PRECONDITION: build() has been called, to create the coding
 *  tree, and initialize root pointer and leaves vector.
 *  THIS METHOD IS USEFUL FOR THE CHECKPOINT BUT SHOULD NOT BE USED
 *  IN THE FINAL SUBMISSION.
 */

int HCTree::decode( ifstream& in ) const {
    if ( root->c0 == nullptr && root->c1 == nullptr ) {
        if ( in.get() == '0' ) {
            return (int)(root->symbol);
        }
        else {
            cout<<"Wrong compressed file!"<<endl;
            return 0;
        }
    }
    else {
        HCNode* temp = root;
        while ( temp->c0 != nullptr && temp->c1 != nullptr ) {
            if ( in.get() == '0' ) {
                temp = temp->c0;
            }
            else {
                temp = temp->c1;
            }
        }
        return (int)(temp->symbol);
    }
}
