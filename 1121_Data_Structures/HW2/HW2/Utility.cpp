#include "Utility.h"


/**
* Desc: Translate the input text into a string from the file.
*
* @param filepath: system path of the input text file.
* @return a string containing the texts from the input file.
*/
string HW2Utility::GetInputTextFromFile(const string &filepath)
{
    ifstream file(filepath, ios::in);
    if (!file) {
        cerr << "Failed to open file: " << filepath << endl;
        return "";
    }
    
    // Load characters from the file one by one and append it into a string.
    string textStr = "";
    char ascii;
    while (file >> std::noskipws >> ascii)
        textStr.push_back(ascii);
    
    return textStr;
}

/**
* Desc: Compute the frequency of each character given a input text.
*
* @param inputText: the input text as a string.
* @param table: the 256-entry frequency table of the characters in input text.
*/
void HW2Utility::ComputeCharFrequencyTable(const string &inputText, int* table)
{
    for (int i = 0; i < inputText.length(); ++i) {
        int charIdx = (int)inputText[i];
        // A character using ASCII code takes 8-bit, thus has a range [0, 255].
        if (charIdx >= 0 && charIdx <= 255)
            table[(int)inputText[i]]++;
        else
            cerr << "ASCII out of range. Please check!" << endl;
    }
}

/**
* Desc: Print the frequency of each non-zero-occurrence character.
*
* @param table: a 256-entry frequency table of the characters in input text.
*/
void HW2Utility::PrintCharFrequencyTable(const int* table)
{
    for (int i = 0; i < FREQ_TABLE_SIZE; ++i) {
        // Only print entry with non-zero occurrence.
        if (table[i] != 0) {
            if (i == 10)
                // Special treatment for the newline symbol.
                cout << "Frequency of Char newline: " << table[10] << endl;
            else 
                cout << "Frequency of Char " << (char)i << ": " << table[i] << endl;
        }
    }
}

/**
* Desc: Check whether two strings are identical.
*
* @param str1: the first string.
* @param str2: the second string.
* @return whether the two strings, origText and result, are identical or not.
*/
bool HW2Utility::CheckIdentical(const string& str1, const string& str2)
{
    return (str1.compare(str2) == 0);
}