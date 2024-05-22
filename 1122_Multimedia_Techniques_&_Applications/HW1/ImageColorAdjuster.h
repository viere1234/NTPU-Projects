#ifndef IMAGE_COLOR_ADJUSTER_H
#define IMAGE_COLOR_ADJUSTER_H

#include "opencv2/opencv.hpp"


// Class ImageColorAdjuster Declaration.
class ImageColorAdjuster
{
public:
	// ImageColorAdjuster Public Methods.
	
	// Constructor.
	ImageColorAdjuster()  {};
	
	// Destructor.
	~ImageColorAdjuster() {};

	// Static method to remap color: Y' = Y^{0.85}.
	static cv::Mat RemapColorCurve1(const cv::Mat &origImg);

	// Static method to remap color: Y' = Y^{1.50}.
	static cv::Mat RemapColorCurve2(const cv::Mat &origImg); 
};


#endif