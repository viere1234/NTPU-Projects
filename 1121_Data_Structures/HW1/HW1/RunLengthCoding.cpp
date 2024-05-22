#include "RunLengthCoding.h"


void RunLengthCoding::EncodeText(const char* origText, const int origLength, TextEncodeData& encodeData)
{
    // Check if the input is valid.
    if (origText == nullptr || origLength <= 0) {
        cerr << "Invalid text input for encoding..." << endl;
        exit(1);
    }
    cout << "Original length: " << origLength << endl;

    // Encode to temporary data vector
    vector<char> tmpData;
    char currentChar = 0;
    int count = 0;
    for (int i = 0; i < origLength; ++i) {
        if (currentChar == *(origText + i)) ++count;
        else {
            // Push previous char datas
            if (count) {
                tmpData.push_back(currentChar);
                string numString = to_string(count);
                tmpData.insert(tmpData.end(), numString.begin(), numString.end());
            }
            // Track new char
            currentChar = *(origText + i);
            count = 1;
        }
    }
    if (count) { // Push last char datas if has
        tmpData.push_back(currentChar);
        string numString = to_string(count);
        tmpData.insert(tmpData.end(), numString.begin(), numString.end());
    }
    tmpData.push_back(0); // Push \0
    
    // Copy temporary data to TextEncodeData
    encodeData.data = new char[tmpData.size()];
    encodeData.dataLength = tmpData.size() - 1;
    copy(tmpData.begin(), tmpData.end(), encodeData.data);

    // Print out encode information (PLEASE DO NOT TOUCH THIS CODE).
    if (encodeData.data != nullptr) {
        cout << "Encode string: " << encodeData.data << endl;
        cout << "Encode length: " << encodeData.dataLength << endl;
        cout << "Compression ratio: " << (float)origLength / (float)encodeData.dataLength << endl;
    }
}

void RunLengthCoding::DecodeText(const TextEncodeData& encodeData, TextDecodeData& decodeData)
{
    // Decode to temporary data vector
    vector<char> tmpData;
    char currentChar = 0;
    int count = 0;
    for (int i = 0; i < encodeData.dataLength; ++i) {
        if (isdigit(*(encodeData.data + i))) count = count * 10 + (*(encodeData.data + i) - '0');
        else {
            // Push previous char datas
            if (count) tmpData.insert(tmpData.end(), count, currentChar);
            // Track new char
            currentChar = *(encodeData.data + i);
            count = 0;
        }
    }
    if (count) tmpData.insert(tmpData.end(), count, currentChar); // Push last char datas if has
    tmpData.push_back(0);


    // Copy temporary data to TextDecodeData
    decodeData.data = new char[tmpData.size()];
    decodeData.dataLength = tmpData.size() - 1;
    copy(tmpData.begin(), tmpData.end(), decodeData.data);

    // Print out decode result (PLEASE DO NOT TOUCH THIS CODE).
    if (decodeData.data != nullptr) {
        cout << "Decode string: " << decodeData.data << endl;
    }
}


void RunLengthCoding::EncodeImage(const cv::Mat& image, ImageEncodeData& encodeData)
{
    // Keep image information (PLEASE DO NOT TOUCH THIS CODE).
    encodeData.imageWidth = image.size().width;
    encodeData.imageHeight = image.size().height;
    encodeData.imageType = image.type();

    // Obtain raw pixel data (PLEASE DO NOT TOUCH THIS CODE).
    uchar* rawImageData = image.data;
    int nPixels = encodeData.imageWidth * encodeData.imageHeight;
    int rawSize = sizeof(uchar) * nPixels;
    cout << "Raw image size: " << rawSize << " bytes" << endl;

    // Encode to temporary data vector
    vector<ImageRunLengthData> tmpData;
    uchar currentUChar = 0;
    int count = 0;
    for (int i = 0; i < nPixels; ++i) {
        if (currentUChar == *(rawImageData + i)) ++count;
        else {
            // Push previous char datas
            if (count) tmpData.emplace_back(currentUChar, count);
            // Track new uchar
            currentUChar = *(rawImageData + i);
            count = 1;
        }
    }
    if (count) tmpData.emplace_back(currentUChar, count); // Push last uchar datas if has

    // Copy temporary data to ImageEncodeData
    encodeData.data = new ImageRunLengthData[tmpData.size()];
    encodeData.numData = tmpData.size();
    copy(tmpData.begin(), tmpData.end(), encodeData.data);
    
    // Print out encode information (PLEASE DO NOT TOUCH THIS CODE).
    int encodedSize = sizeof(ImageRunLengthData) * encodeData.numData;
    cout << "Encoded size: " << encodedSize << " bytes" << endl;
    cout << "Compression ratio: " << (float)rawSize / (float)encodedSize << endl;
}

void RunLengthCoding::DecodeImage(const ImageEncodeData& encodeData, cv::Mat& decodeImage)
{
    // Decode to temporary data vector
    vector<uchar> tmpData;
    for (int i = 0; i < encodeData.numData; ++i)
        tmpData.insert(tmpData.end(), (encodeData.data + i)->count, (encodeData.data + i)->value);

    // Copy temporary data to decodeData
    uchar* decodeData = new uchar[encodeData.imageWidth * encodeData.imageHeight];
    copy(tmpData.begin(), tmpData.end(), decodeData);
    
    // Fill the data in _decodeData_ into OpenCV Mat (PLEASE DO NOT TOUCH THIS CODE).
    decodeImage = cv::Mat(cv::Size(encodeData.imageWidth, encodeData.imageHeight), encodeData.imageType);
    for (int h = 0; h < decodeImage.size().height ; ++h) {
        for (int w = 0; w < decodeImage.size().width; ++w) {
            decodeImage.at<uchar>(h, w) = decodeData[h * decodeImage.size().width + w];
        }
    }
}
