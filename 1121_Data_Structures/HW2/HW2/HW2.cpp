/*
    Homework assignment #2 of Data Structure 2023, NTPU CSIE.
    Goals: implementaton of Huffman coding.
*/

#include "Global.h"
#include "Utility.h"
#include "HuffmanCoding.h"

const int NumTestCases = 4;
const char* testInputFiles[NumTestCases] = { 
    "TestInputs/Simple.txt",
    "TestInputs/HarryPorter.txt",
    "TestInputs/HarryPorter2.txt",
    "TestInputs/HuffmanCodingWiki.txt"
};


int main()
{
    int numPasses = 0;
    int numFails  = 0;
    // Test all input cases.
    for (int i = 0; i < NumTestCases; ++i) {
        // For each test case, encode the input text with huffman encoding and then decode it back.
        // Get test input.
        cout << "Run testcase #" << i << endl;
        string inputText = HW2Utility::GetInputTextFromFile(testInputFiles[i]);
        if (inputText.length() > 0) {
            
            // Build the frequency table for the characters in the text.
            int charFreqTable[FREQ_TABLE_SIZE] = { 0 };
            HW2Utility::ComputeCharFrequencyTable(inputText, charFreqTable);
            // HW2Utility::PrintCharFrequencyTable(charFreqTable);
        
            // Create a HuffmanCoding instance.
            HuffmanCoding huffmanCoding(charFreqTable);
            // huffmanCoding.PrintEncodingTable();
        
            // Perform huffman encoding with the table and output a binary string.
            string encodedStr = huffmanCoding.Encode(inputText);
            // cout << "Encoded String: " << endl;
            // cout << encodedStr << endl;

            // Perform huffman decoding and output the original text paragraph.
            string decodedStr = huffmanCoding.Decode(encodedStr);
            cout << "Decoded String: " << endl;
            cout << decodedStr << endl;

            // Check correctness and print result.
            cout << "----------------------------" << endl;
            if (HW2Utility::CheckIdentical(inputText, decodedStr)) {
                cout << "Test case #" << i << " passed!" << endl;
                numPasses++;
            }
            else {
                cout << "Test case #" << i << " failed!" << endl;
                numFails++;
            }
            cout << "----------------------------" << endl;
        }
        else
            cout << "Empty input text ... " << endl;
    }
    // Print overall test results.
    cout << endl;
    cout << NumTestCases << " test cases: " << numPasses << " passed, " << numFails << " failed" << endl;
    return 0;
}


