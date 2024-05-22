#include "HuffmanCoding.h"


/**
* Constructor: Build a huffman tree and generate an encoding table with the tree.
*
* @param charFreqTable: a 256-entry frequency table of the characters in input text.
*/
HuffmanCoding::HuffmanCoding(const int* charFreqTable)
{
	// Create an initial node list with all characters with non-zero min frequency priority list.
	const auto frequencyCmp = [](HuffmanTreeNode* a, HuffmanTreeNode* b) { return a->frequency > b->frequency; };
	priority_queue<HuffmanTreeNode*, std::vector<HuffmanTreeNode*>, decltype(frequencyCmp)> nodeList(frequencyCmp);
	for (int i = 0; i < FREQ_TABLE_SIZE; ++i) {
		if (charFreqTable[i]) { // only non-zero
			HuffmanTreeNode* pointer = new HuffmanTreeNode(i, charFreqTable[i]);
			nodeList.push(pointer);
		}
	}
	numChars = nodeList.size();

	// Build the huffman tree in bottom-up manner.
	while (nodeList.size() > 1) {
		HuffmanTreeNode* left = nodeList.top();
		nodeList.pop();
		HuffmanTreeNode* right = nodeList.top();
		nodeList.pop();
		HuffmanTreeNode* newNode = new HuffmanTreeNode(0, left->frequency + right->frequency, left, right);
		nodeList.push(newNode);
	}
	root = nodeList.top();		
	
	// Generate encoding and decoding Tables
	std::string code;
	GenerateCodingTable(root, code);
}

/**
* Destructor: Release memory when the HuffmanCoding instance has been destroyed.
*/
HuffmanCoding::~HuffmanCoding()
{ ReleaseHuffmanTree(root); }

/**
* Desc: Generate the encoding table by traverse the huffman tree by postorder.
*/
void HuffmanCoding::GenerateCodingTable(const HuffmanTreeNode* root, std::string& code) 
{
	if (!root->leftChild && !root->rightChild) {
		encodingTable[root->ch] = code;
		decodingTable[code] = root->ch;
		return;
	}
	if (root->leftChild) {
		code.push_back('0');
		GenerateCodingTable(root->leftChild, code);
		code.pop_back();
	}
	if (root->rightChild) {
		code.push_back('1');
		GenerateCodingTable(root->rightChild, code);
		code.pop_back();
	}
}

/**
* Desc: Release memory of all nodes in the Huffman tree by postorder.
*/
void HuffmanCoding::ReleaseHuffmanTree(const HuffmanTreeNode* root)
{
	if (!root->leftChild && !root->rightChild) {
		delete root;
		return;
	}
	if (root->leftChild) ReleaseHuffmanTree(root->leftChild);
	if (root->rightChild) ReleaseHuffmanTree(root->rightChild);
}

/**
* Desc: Print the encoding table.
*/
void HuffmanCoding::PrintEncodingTable() const
{
	for (const auto& x : encodingTable)
		std::cout << x.first << ": " << x.second << std::endl;
}

/**
* Desc: Encode an input string using the encoding table..
*
* @param inputText: the input text for encoding.
* @return the encoded string (bit pattern, stored as a string for convenience).
*/
string HuffmanCoding::Encode(const string inputText)
{
	// Compute original size.
	cout << "Original Size: " << ceil(log2(numChars)) * inputText.length() << " bits" << endl;

	// Encode
	string encodedStr = "";
	for (const auto& c : inputText)
		encodedStr.append(encodingTable[c]);

	// Print encoded size.
	cout << "Encoded Size: " << encodedStr.length() << " bits" << endl;

	return encodedStr;
}

/**
* Desc: Decode the encoded string (bit pattern) into the original string using the HuffmanTree.
*
* @param encodedText: the encoded bit pattern (stored as a string for convenience).
* @return the decoded result.
*/
string HuffmanCoding::Decode(const string encodedText)
{
	string decodedStr = "";
	string currentStr;
	for (const auto& c : encodedText) {
		if (currentStr.size() && decodingTable.count(currentStr)) {
			decodedStr.push_back(decodingTable[currentStr]);
			currentStr.clear();
		}
		currentStr.push_back(c);
	}
	decodedStr.push_back(decodingTable[currentStr]);
	
	return decodedStr;
}

