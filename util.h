//
//  Project 5 - mymap
//  Course: CS 251, fall 2022. Wed 12pm lab
//  System: Clion
//  Author: Juan Miguel Cruz
//  Net Id: Jcruz85
// cite:
// Juan, project 5 fall 2022
//
#pragma once

#include <iostream>
#include "hashmap.h"
#include "bitstream.h"
#include <queue>

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    HuffmanNode* zero;
    HuffmanNode* one;
};

struct compare
{
    bool operator()(const HuffmanNode *lhs,
                    const HuffmanNode *rhs)
    {
        return lhs->count > rhs->count;
    }
};

//
// *This method frees the memory allocated for the Huffman tree.
//
void freeTree(HuffmanNode* node) {
    if(node == nullptr){ // checks if the tree is an empty tree
        return;
    }
    // using post order traversal to delete all the nodes from the tree
    freeTree(node->zero);
    freeTree(node->one);
    delete node; // deletes the node

}

//
// *This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//

void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    string str;
    HuffmanNode temp;
    ifstream infile;
    infile.open(filename);// opens the file
    char c;

    if(isFile){ // checks if it's a file
        while(infile.get(c)){ // while there's still a character to the file
            temp.character =c;
            if(map.containsKey(c)){ // checks if the map has the key
                map.put(temp.character, map.get(temp.character)+1); // this inserts the character and the value of the node to the map
            }
            else{
                temp.count = 1;// initializes the value to one
                map.put(temp.character,temp.count); // inserts the node "character" and a value of one
            }
        }
    }
    else{ // this is for the string
        for(char c : str){ //
            temp.character =c;
            if(map.containsKey(c)){// checks if the map has the key
                map.put(temp.character, map.get(temp.character)+1);// this inserts the character and the value of the node to the map
            }
            else{
                temp.count = 1; // initializes the value to one
                map.put(temp.character,temp.count); // inserts the node "character" and a value of one
            }
        }
    }
    map.put(256, 1); // inserts the PSEUDO_EOF
}

//
// *This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    class prioritize{ // a class for the priority queue
    public:
        bool operator()(const HuffmanNode* p1, const HuffmanNode* p2) const{
            return p1->count > p2->count; // this sorts the priority queue
        }
    };
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, prioritize> pq; //this is the priority queue
    HuffmanNode* node;
    for(int key : map.keys()){ // creates the priority queue
        node = new HuffmanNode; // creates the new node
        node->character = key; // the node character is the tree
        node->count = map.get(key); // the value of the node
        node->zero = nullptr; //initializes the left pointer of the node to nullptr
        node->one = nullptr;//initializes the right pointer of the node to nullptr
        pq.push(node); // inserts the node to the priority queue
    }
    HuffmanNode* temp;
    HuffmanNode* first;
    HuffmanNode* second;
    while(!pq.empty()){ // this is for creating the tree
        first = pq.top(); // stores the very first node
        pq.pop(); // delete the first node
        second = pq.top(); // // stores the very first node after popping the first one
        pq.pop(); // delete the first node
        temp = new HuffmanNode; // creates a new node
        temp->count = first->count + second->count; // the value of the new node is the sum of the value of the first and second node
        temp->zero = first; //the left of the new node created is the first node that was pop
        temp->one = second;//the right of the new node created is the second node that was pop
        temp->character = NOT_A_CHAR; // setting the character to NOT_A_CHAR
        pq.push(temp); // inserts the node
        if(pq.size() == 1){ // checks if the size is one meaning it hits the root
            break;
        }
    }
    return temp; // returns the temp which is the tree
}


//
// *Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str,
                       HuffmanNode* prev) {
    if(node == nullptr){ // checks if it's an empty tree
        return;
    }
    if (node->character != NOT_A_CHAR){ // checks for a character
        encodingMap.emplace(node->character, str);// inserts the character and the string(binary numbers)
    }
    //calling it recursively and adding 0 or 1 to the str
    _buildEncodingMap(node->zero, encodingMap, str + "0", prev);
    _buildEncodingMap(node->one, encodingMap, str + "1", prev);

}

//
// *This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    string str;
    _buildEncodingMap(tree, encodingMap, str, tree); // calls the helper function
    return encodingMap;
}

//
// *This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output,
              int &size, bool makeFile) {
    string str;
    char c;
    if(makeFile){
        while(input.get(c)){// while there's still  a character
            str += encodingMap.at(c); // gets the value and store it to str
        }
        str += encodingMap.at(PSEUDO_EOF); // returns the value of PSEUDO_EOF
    }

    for(int i=0; i< str.length(); i++){
        int num;
        if(str[i] == '0'){ // converts the char '0' to an int
            num = 0;
        }
        else{ // converts the char '1' to an int
            num = 1;
        }
        output.writeBit(num); //writes the bit to the ostream
        size ++; // increments the size
    }
    return str; // returns the whole string of the binary numbers of the encoded file
}

//
// *This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    string str;
    HuffmanNode* root = encodingTree; // holder for the root

    while(!input.eof()){
        int bit = input.readBit(); // reads in the bit
        if(encodingTree->character ==  PSEUDO_EOF){ // checks for the PSEUDO_EOF
            break;
        }
        if(encodingTree->character != NOT_A_CHAR) { //checks for the character
            str += encodingTree->character; // stores(concatenate) the character
            output.put(encodingTree->character); // stores the characters to output
            encodingTree = root; // returns back to the root
        }
        if(bit == 0){ // traverse left if it's zero
            encodingTree = encodingTree->zero;
        }
        else{ // travers right otherwise
            encodingTree = encodingTree->one;
        }
    }
    return str; // returns all the characters
}

//
// *This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    //initialization
    HuffmanNode* node;
    bool isFile = true;
    hashmap frequencyMap;
    hashmapE encodingMap;
    string str;
    int size;

    buildFrequencyMap(filename, isFile, frequencyMap); // builds a frequency map using the function
    ofbitstream output (filename + ".huf"); //creates a new file .huf
    output << frequencyMap; //outputs the frequency map
    node = buildEncodingTree(frequencyMap); // builds a tree
    encodingMap = buildEncodingMap(node); // builds an encoding map
    ifstream input(filename); // takes in the filename
    str = encode(input, encodingMap, output, size, isFile); // creates the encoded string
    freeTree(node); // frees the tree
    return str;
}

//
// *This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note: this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    //initialization
    string substr, str;
    hashmap frequencyMap;
    HuffmanNode* node;

    stringstream ss(filename); // for parsing
    getline(ss, substr, '.'); // parse the filename

    ifbitstream input(filename); // takes the file name
    input >> frequencyMap; //inputs the frequency map
    node = buildEncodingTree(frequencyMap); //builds the tree from calling the buildEncodingTree function
    ofstream output(substr + "_unc.txt"); // creates a new txt file
    str = decode(input, node, output); // calls the decode function and store it to str
    freeTree(node); // free the tree
    return str;
}

