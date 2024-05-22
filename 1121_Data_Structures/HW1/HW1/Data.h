#ifndef DATA_H
#define DATA_H

#include <opencv2/opencv.hpp>


// Text coding data.
// --------------------------------------------------------------------
// Struct TextEncodeData Declaration.
struct TextEncodeData
{
    TextEncodeData()
    {
        data = nullptr;
        dataLength = 0;
    }
    ~TextEncodeData()
    {
        if (data != nullptr) {
            delete [] data;
            data = nullptr;
        }
    }
    char* data;         // The encoded char data stream.
    int dataLength;     // The length of the encoded char data stream.
};

// Struct TextDecodeData Declaration.
struct TextDecodeData
{
    TextDecodeData()
    {
        data = nullptr;
        dataLength = 0;
    }
    ~TextDecodeData()
    {
        if (data != nullptr) {
            delete [] data;
            data = nullptr;
        }
    }
    char* data;         // The decoded char data stream.
    int dataLength;     // The length of the decoded char data stream.
};



// Image coding data.
// --------------------------------------------------------------------
// Struct ImageRunLengthData Declarations.
struct ImageRunLengthData
{
    ImageRunLengthData()
    {
        value = 0;
        count = 0;
    }
    ImageRunLengthData(uchar val, int c)
    {
        value = val;
        count = c;
    }
    uchar value;        // Pixel value.
    int count;          // The successive occurrence of the pixel value.
};

// Struct ImageEncodeData Declarations.
struct ImageEncodeData
{
    ImageEncodeData()
    {
        data = nullptr;
        numData = imageWidth = imageHeight = imageType = 0;
    }
    ~ImageEncodeData()
    {
        if (data != nullptr) {
            delete [] data;
            data = nullptr;
        }
    }
    ImageRunLengthData* data;   // Encoded ImageRunLengthData data array.
    int numData;                // The number of items in the encoded data array.

    // Data for reconstructing the original image.
    int imageWidth;     // Original image width.
    int imageHeight;    // Original image height.
    int imageType;      // Original image type.
};

#endif

