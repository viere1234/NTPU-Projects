#include "ImageScaler.h"
#include "VectorTools.h"

#include <algorithm>

cv::Mat ImageScaler::Resize(const cv::Mat &origImg, const int newWidth, const int newHeight)
{
	const cv::Size newSize(newWidth, newHeight);
	cv::Mat editImg = cv::Mat(newSize, origImg.type());
	
	// Calculate the scalling factors for both X and Y.
	const cv::Vec2d scaleFactorXY(
		static_cast<double>(newWidth) / origImg.size().width, 
		static_cast<double>(newHeight) / origImg.size().height
	);

	// Iterate through every pixel in the scaled image.
	for (int height = 0; height < newHeight; ++height)
		for (int width = 0; width < newWidth; ++width) {
			// Get the inversed coordinate XY in the original image.
			const cv::Vec2d srcXY(width / scaleFactorXY[0], height / scaleFactorXY[1]);

			// Get two coordinates for the four interpolated points.
			cv::Vec3i p1, p4;
			p1[0] = static_cast<int>(srcXY[0]);
			p1[1] = static_cast<int>(srcXY[1]);
			p4[0] = p1[0] + 1;
			p4[1] = p1[1] + 1;

			// Make sure the interpolated points coordinates are within the image bounds.
			p1[0] = clamp(p1[0], 0, origImg.size().width - 1);
			p1[1] = clamp(p1[1], 0, origImg.size().height - 1);
			p4[0] = clamp(p4[0], 0, origImg.size().width - 1);
			p4[1] = clamp(p4[1], 0, origImg.size().height - 1);

			// Calculate the interpolation weights.
			const double a = srcXY[0] - p1[0];
			const double b = srcXY[1] - p1[1];

			// Perform bilinear interpolation and the store the result.
			editImg.at<cv::Vec3b>(height, width) = static_cast<cv::Vec3b>(
				(1 - a) * (1 - b) * cv::Vec3d(origImg.at<cv::Vec3b>(p1[1], p1[0])) +
				a * (1 - b) * cv::Vec3d(origImg.at<cv::Vec3b>(p1[1], p4[0])) + 
				(1 - a) * b * cv::Vec3d(origImg.at<cv::Vec3b>(p4[1], p1[0])) +
				a * b * cv::Vec3d(origImg.at<cv::Vec3b>(p4[1], p4[0]))
			);
		}
	return editImg;
}

