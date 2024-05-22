#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;


// Class ImageUtil Declaration.
class ImageUtil
{
// ImageUtil Public Methods.
public:
	ImageUtil() {};
	~ImageUtil() {};

    // Desc: Downsize an image and show it in a window titled with windowTitle.
	static void ShowImage(cv::Mat& image, string windowTitle) {
        // Determine whether to downsize the image for displaying.
        const float downSizeRatio = image.size().width > 1024 ? 0.2f : 1.0f;
        cv::Size size = image.size();
        int showWidth  = (int)(downSizeRatio * size.width);
        int showHeight = (int)(downSizeRatio * size.height);
        cv::Mat showImg = cv::Mat(cv::Size(showWidth, showHeight), image.type());
        cv::resize(image, showImg, cv::Size(showWidth, showHeight));
        cv::imshow(windowTitle, showImg);
        cv::waitKey(0);
	}

    // Desc: Check whether two uchar images are the same.
    static bool SameUCharImage(cv::Mat& image1, cv::Mat& image2) {
        // Check image resolutions.
        if (image1.size().width != image2.size().width ||
            image1.size().height != image2.size().height) {
            cerr << "Image resolutions not matched" << endl;
            return false;
        }
        // Check image type.
        if (image1.type() != CV_8UC1 || image2.type() != CV_8UC1) {
            cerr << "Image is not with type uchar" << endl;
            return false;
        }
        // Check image pixel values.
        for (int h = 0; h < image1.size().height; ++h) {
            for (int w = 0; w < image1.size().width; ++w) {
                if (image1.at<uchar>(h, w) != image2.at<uchar>(h, w))
                    return false;
            }
        }
        return true;
    }
};
