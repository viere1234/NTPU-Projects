#include "ImageFilter.h"
#include "VectorTools.h"
#include <vector>
#include <cmath>

#include <iostream>

cv::Mat GaussianFilter::Filter(const cv::Mat& img)
{
	const int radius = (int)ceil(2 * spatialSigma);
	const int kernelSize = 2 * radius + 1;

	cv::Mat fltImg = cv::Mat(img.size(), img.type());
	
	// Create Gaussian kernel
	cv::Mat kernel = cv::Mat(kernelSize, kernelSize, CV_64F);
	double weightSum = 0.0; // For Normalization (sum of weight = 1).
	for (int y = -radius; y <= radius; ++y)
		for (int x = -radius; x <= radius; ++x) {
			double value = std::exp((x * x + y * y) / (-2 * spatialSigma * spatialSigma));
			kernel.at<double>(y + radius, x + radius) = value;
			weightSum += value;
		}

	// Normalize filter kernel
	kernel /= weightSum;

	// Perform Gaussian filter on the image.
	cv::filter2D(img, fltImg, -1, kernel, cv::Point(-1, -1), 0.0, CV_HAL_BORDER_REFLECT_101);
	
	return fltImg;
}


cv::Mat BilateralFilter::Filter(const cv::Mat& img)
{
	const int radius = (int)ceil(2 * spatialSigma);
	const int kernelSize = 2 * radius + 1;
		
	cv::Mat fltImg = cv::Mat(img.size(), img.type());
	
	// Pre-calculate the Gaussian weights for range distance (0 ~ 255).
	std::vector<double> rangeWeights(256);
	for (int i = 0; i < 256; ++i)
		rangeWeights[i] = std::exp(-i * i / (2 * rangeSigma * rangeSigma));

	// Pre-calculate the Gaussian weights for spatial distance.
	cv::Mat spatialWeights = cv::Mat(kernelSize, kernelSize, CV_64F);
	for (int y = -radius; y <= radius; ++y)
		for (int x = -radius; x <= radius; ++x)
			spatialWeights.at<double>(y + radius, x + radius) = std::exp((x * x + y * y) / (-2 * spatialSigma * spatialSigma));

	// For converting the RGB image to gray scale for intensity difference on the fly with filter.
	cv::Mat grayImg(img.size(), CV_8U);
	cv::Mat grayValueReady = cv::Mat::zeros(img.size(), CV_8U);
	const cv::Vec3d BGR2Gray(0.114, 0.587, 0.299);

	// Perform bilateral filter on each pixel.
	for (int y = 0; y < img.rows; ++y)
		for (int x = 0; x < img.cols; ++x) {
			// Get the gray value of the current pixel.
			if (grayValueReady.at<uchar>(y, x) == 0) {
				grayImg.at<uchar>(y, x) = static_cast<uchar>(img.at<cv::Vec3b>(y, x).dot(BGR2Gray));
				grayValueReady.at<uchar>(y, x) = 1;
			}
			const uchar centerColorIntensity = grayImg.at<uchar>(y, x);

			cv::Vec3d colorSum(0.0, 0.0, 0.0);
			double weightSum = 0.0;
			
			// Iterative through the neighbors by the radious
			for (int j = -radius; j <= radius; ++j)
				for (int i = -radius; i <= radius; ++i) {
					// Get the pixel coordinate and check if is within the image bound.
					const cv::Vec2i neighborXY(i + x, j + y);
					if (neighborXY[0] < 0 || neighborXY[0] >= img.cols || neighborXY[1] < 0 || neighborXY[1] >= img.rows)
						continue;

					// Check if the current neighbor pixel had gray value pre-calculated or calculate now.
					if (grayValueReady.at<uchar>(neighborXY[1], neighborXY[0]) == 0) {
						grayImg.at<uchar>(neighborXY[1], neighborXY[0]) = static_cast<uchar>(img.at<cv::Vec3b>(neighborXY[1], neighborXY[0]).dot(BGR2Gray));
						grayValueReady.at<uchar>(neighborXY[1], neighborXY[0]) = 1;
					}

					// Calculate the corresponded weight
					const double weight = (
						rangeWeights[std::abs(grayImg.at<uchar>(neighborXY[1], neighborXY[0]) - centerColorIntensity)] * // Range weight
						spatialWeights.at<double>(j + radius, i + radius) // Spatial weight
					);

					colorSum += weight * static_cast<cv::Vec3d>(img.at<cv::Vec3b>(neighborXY[1], neighborXY[0]));
					weightSum += weight;
				}

			// Normalize the color result (within 0 ~ 255) and store.
			fltImg.at<cv::Vec3b>(y, x) = static_cast<cv::Vec3b>(colorSum / weightSum);
		}
	
	return fltImg;
}
