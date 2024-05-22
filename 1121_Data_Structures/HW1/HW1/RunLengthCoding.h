#ifndef RUN_LENGTH_CODING_H
#define RUN_LENGTH_CODING_H

#include "Data.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;

// Class RunLengthCoding Declarations.
class RunLengthCoding
{
public:
	// RunLengthCompression Public Methods.
	RunLengthCoding() {};
	~RunLengthCoding() {};
	
	// Text Encoding/Decoding Methods.
	static void EncodeText(const char* text, const int length, TextEncodeData& encodeData);
	static void DecodeText(const TextEncodeData& encodeData, TextDecodeData& decodeData);

	// Image Encoding/Decoding Methods.
	static void EncodeImage(const cv::Mat &image, ImageEncodeData& encodeData);
	static void DecodeImage(const ImageEncodeData &encodeData, cv::Mat& decodeImage);
};


#endif
