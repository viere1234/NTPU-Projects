/*
    Homework assignment #1 of Data Structure 2023, NTPU CSIE.
    Goals: implementaton of text/image run-length coding.
*/

#include "RunLengthCoding.h"
#include "ImageUtil.h"

// Global variables.
const float waitTime = 1.0f;    // For delaying the next test.

// Test text inputs.
const int nTestTexts = 3;
const char* testTexts[nTestTexts] = {
    "wwwwaaadexxxxxx",
    "aaaaaaaaaaaaaaaaaaabbbbbbbbccccccccccccccccccccccccccccccccccddd",
    "pneumonoultramicroscopicsilicovolcanoconiosis"
};

// Test image inputs.
const int nTestImages = 3;
const char* testImageFiles[nTestImages] = {
    "TestInputs/Simple.png",
    "TestInputs/SpyFamily.png",
    "TestInputs/AttackOnTitan.png"
};

// Method prototypes.
bool ValidateTextCoding(const TextDecodeData&, const char*, const int);
void RunTextCodingCheck();
void RunImageCodingCheck();
void Sleep(float seconds);



int main(int argc, char** argv)
{
    // Part1: text encoding/decoding.
    RunTextCodingCheck();

    cout << endl;
    cout << "=================================================================" << endl;
    cout << endl;

    // Part2: image encoding/decoding.
    RunImageCodingCheck();


    return 0;
}

void RunTextCodingCheck()
{
    for (int i = 0; i < nTestTexts; ++i) {
        cout << "Check text input " << i << ": " << endl;
        
        // Load the test text.
        const char* testText = testTexts[i];
        
        // Perform encoding.
        TextEncodeData textEncodeData;
        RunLengthCoding::EncodeText(testText, (int)strlen(testText), textEncodeData);
        
        // Perform decoding.
        TextDecodeData textDecodeData;
        RunLengthCoding::DecodeText(textEncodeData, textDecodeData);
        
        // Validation of text encoding/decoding.
        if (ValidateTextCoding(textDecodeData, testText, (int)strlen(testText)))
            cout << "[RESULT] test text input " << i << " passed!" << endl;
        else
            cout << "[RESULT] test text input " << i << " failed..." << endl;
        
        cout << "-------------------------------------------------------------" << endl;
        Sleep(waitTime);
    }
}

void RunImageCodingCheck()
{
    for (int i = 0; i < nTestImages; ++i) {
        cout << "Check image input " << i << ": " << endl;
        
        // Load the test image.
        cv::Mat origImg = cv::imread(testImageFiles[i], cv::IMREAD_GRAYSCALE);
        // ImageUtil::ShowImage(origImg, "Image before comprssion (downsampled)");
        
        // Perform encoding.
        ImageEncodeData imgEncodeData;
        RunLengthCoding::EncodeImage(origImg, imgEncodeData);
        
        // Perform decoding.
        cv::Mat decodeImg;
        RunLengthCoding::DecodeImage(imgEncodeData, decodeImg);
        // ImageUtil::ShowImage(decodeImg, "Image after comprssion (downsampled)");
        
        // Validation of image encoding/decoding.
        if (ImageUtil::SameUCharImage(origImg, decodeImg))
            cout << "[RESULT] test image input " << i << " passed!" << endl;
        else
            cout << "[RESULT] test image input " << i << " failed..." << endl;
        
        cout << "-------------------------------------------------------------" << endl;
        Sleep(waitTime);
    }
}

bool ValidateTextCoding(const TextDecodeData& decodeData, const char* origText, const int origLength)
{
    if (decodeData.dataLength != origLength)
        return false;

    for (int i = 0 ; i < decodeData.dataLength ; ++i)
        if (decodeData.data[i] != origText[i])
            return false;

    return true;
}

void Sleep(float seconds)
{
    clock_t startClock = clock();
    float secondsAhead = seconds * CLOCKS_PER_SEC;
    // Do nothing until the elapsed time has passed.
    while(clock() < startClock+secondsAhead);
}


