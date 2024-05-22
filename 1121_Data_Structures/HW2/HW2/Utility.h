#ifndef UTILITY_H
#define UTILITY_H

#include "Global.h"

// Class Utility Declaration.
class HW2Utility
{
public:
	// Utility Public Methods.	
	HW2Utility()  {};
	~HW2Utility() {};

	/**
	* Desc: Translate the input text into a string from the file.
	*
	* @param filepath: system path of the input text file.
	* @return a string containing the texts from the input file.
	*/
	static string GetInputTextFromFile(const string &filepath);
	
	/**
	* Desc: Compute the frequency of each character given a input text.
	*
	* @param inputText: the input text as a string.
	* @param table: the 256-entry frequency table of the characters in input text.
	*/
	static void ComputeCharFrequencyTable(const string &inputText, int* table);
	
	/**
	* Desc: Print the frequency of each non-zero-occurrence character.
	*
	* @param table: a 256-entry frequency table of the characters in input text.
	*/
	static void PrintCharFrequencyTable(const int* table);
	
	/**
	* Desc: Check whether two strings are identical.
	*
	* @param str1: the first string.
	* @param str2: the second string.
	* @return whether the two strings, origText and result, are identical or not.
	*/
	static bool CheckIdentical(const string &str1, const string &str2);
};



#endif
