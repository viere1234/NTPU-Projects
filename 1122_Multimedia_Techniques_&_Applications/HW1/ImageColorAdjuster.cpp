#include "ImageColorAdjuster.h"
#include "VectorTools.h"
#include <iostream>

cv::Mat ImageColorAdjuster::RemapColorCurve1(const cv::Mat& image)
{
	cv::Mat newImage = cv::Mat(image.size(), image.type());

	int width  = image.cols;
	int height = image.rows;
	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			// Get pixel color.
			cv::Vec3d BGR = static_cast<cv::Vec3d>(image.at<cv::Vec3b>(h, w)) / 255.0;
			
			// RGB to YCbCr.
			cv::Vec3d YCbCr;
			YCbCr[0] = 0.299 * BGR[2] + 0.587 * BGR[1] + 0.114 * BGR[0]; // Y = 0.299R + 0.587G + 0.114B
			YCbCr[1] = 0.568 * (BGR[0] - YCbCr[0]) + 128;                // Cb = 0.568(B - Y) + 128
			YCbCr[2] = 0.713 * (BGR[2] - YCbCr[0]) + 128;                // Cr = 0.713(R - Y) + 128

			// Scale the luminance channel. Y' = Y^{0.85}
			YCbCr[0] = cv::pow(YCbCr[0], 0.85);
			
			// YCbCr to RGB.
			BGR[2] = YCbCr[0] + 1.371 * (YCbCr[2] - 128);                            // R = Y + 1.371(Cr - 128)
			BGR[1] = YCbCr[0] - 0.698 * (YCbCr[2] - 128) - 0.336 * (YCbCr[1] - 128); // G = Y - 0.698(Cr - 128) - 0.336(Cb - 128)
			BGR[0] = YCbCr[0] + 1.732 * (YCbCr[1] - 128);                            // B = Y + 1.732(Cb - 128)

			// Make sure the result is in range of 0.0 and 1.0.
			clampVec3(BGR, 0.0, 1.0);
			
			// Write the new color to newImage.
			newImage.at<cv::Vec3b>(h, w) = static_cast<cv::Vec3b>(BGR * 255.0);
		}
	}
	return newImage;
}


cv::Mat ImageColorAdjuster::RemapColorCurve2(const cv::Mat& image)
{
	cv::Mat newImage = cv::Mat(image.size(), image.type());
	
	int width  = image.cols;
	int height = image.rows;
	for (int h = 0; h < height; ++h) {
		for (int w = 0; w < width; ++w) {
			// Get pixel color.
			cv::Vec3d BGR = cv::Vec3d(image.at<cv::Vec3b>(h, w)) / 255.0;
			
			// RGB to YCbCr.
			cv::Vec3d YCbCr;
			YCbCr[0] = 0.299 * BGR[2] + 0.587 * BGR[1] + 0.114 * BGR[0]; // Y = 0.299R + 0.587G + 0.114B
			YCbCr[1] = 0.568 * (BGR[0] - YCbCr[0]) + 128;                // Cb = 0.568(B - Y) + 128
			YCbCr[2] = 0.713 * (BGR[2] - YCbCr[0]) + 128;                // Cr = 0.713(R - Y) + 128

			// Scale the luminance channel. Y' = Y^{1.50}
			YCbCr[0] = cv::pow(YCbCr[0], 1.50);

			// YCbCr to RGB.
			BGR[2] = YCbCr[0] + 1.371 * (YCbCr[2] - 128);                            // R = Y + 1.371(Cr - 128)
			BGR[1] = YCbCr[0] - 0.698 * (YCbCr[2] - 128) - 0.336 * (YCbCr[1] - 128); // G = Y - 0.698(Cr - 128) - 0.336(Cb - 128)
			BGR[0] = YCbCr[0] + 1.732 * (YCbCr[1] - 128);                            // B = Y + 1.732(Cb - 128)
			
			// Make sure the result is in range of 0.0 and 1.0.
			clampVec3(BGR, 0.0, 1.0);

			// Write the new color to newImage.
			newImage.at<cv::Vec3b>(h, w) = cv::Vec3b(BGR * 255.0);
		}
	}
	return newImage;
}