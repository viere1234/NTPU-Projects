#ifndef HUFFMAN_CODING_H
#define HUFFMAN_CODING_H

#include "Global.h"

// Struct HuffmanTreeNode Declaration.
struct HuffmanTreeNode
{
	HuffmanTreeNode(const char c, int freq, 
					HuffmanTreeNode* left = nullptr, 
					HuffmanTreeNode* right = nullptr)
	{
		ch = c;
		frequency = freq;
		leftChild = left;
		rightChild = right;
	};
	// The character.
	char ch;
	// The frequency (# occurrence) of the character in the text.
	int frequency;
	HuffmanTreeNode* leftChild;
	HuffmanTreeNode* rightChild;
};

// Class HuffmanCoding Declaration.
class HuffmanCoding
{
public:
	// HuffmanCoding Public Methods.
	
	/**
	* Constructor: Build a huffman tree and generate an encoding table with the tree.
	*
	* @param charFreqTable: a 256-entry frequency table of the characters in input text.
	*/
	HuffmanCoding(const int* charFreqTable);
	
	/**
	* Destructor: Release memory when the HuffmanCoding instance has been destroyed.
	*/
	~HuffmanCoding();

	/**
	* Desc: Print the encoding table.
	*/
	void PrintEncodingTable() const;

	/**
	* Desc: Encode an input string using the encoding table..
	*
	* @param inputText: the input text for encoding.
	* @return the encoded string (bit pattern, stored as a string for convenience).
	*/
	string Encode(const string inputText);
	
	/**
	* Desc: Decode the encoded string (bit pattern) into the original string using the HuffmanTree.
	*
	* @param encodedText: the encoded bit pattern (stored as a string for convenience).
	* @return the decoded result.
	*/
	string Decode(const string encodedText);

private:
	// HuffmanCoding Private Methods.
	void GenerateCodingTable(const HuffmanTreeNode*, std::string&);
	void ReleaseHuffmanTree(const HuffmanTreeNode*);

	// HuffmanCoding Private Data.
	// The HuffmanTree.
	HuffmanTreeNode* root;
	// The built encoding decoding table.
	unordered_map<char, std::string> encodingTable;
	unordered_map<std::string, char> decodingTable;
	// Number of different characters in text.
	int numChars;
};


#endif
