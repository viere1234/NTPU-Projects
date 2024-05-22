#ifndef IMAGE_SCALER_H
#define IMAGE_SCALER_H

#include "opencv2/opencv.hpp"


// Class ImageScaler Declaration.
class ImageScaler
{
public:
	// ImageScaler Public Methods.
	
	// Constructor.
	ImageScaler()  {};
	
	// Destructor.
	~ImageScaler() {};

	// Static method to resize a given image with the new width and height.
	static cv::Mat Resize(const cv::Mat &origImg, const int newWidth, const int newHeight);
};


#endif