#ifndef IMAGE_FILTER_H
#define IMAGE_FILTER_H

#include "opencv2/opencv.hpp"


// Class ImageFilter Declaration (Base Class).
class ImageFilter
{
public:
	// ImageFilter Public Methods. 
	
	// Constructor.
	ImageFilter()  {};
	
	// Destructor.
	~ImageFilter() {};

	// Pure virtual function for performing filtering.
	virtual cv::Mat Filter(const cv::Mat &img) = 0;
};


// Class GaussianFilter Declaration.
class GaussianFilter : public ImageFilter
{
public:
	// GaussianFilter Public Methods.
	
	// Constructor.
	GaussianFilter(const float sigmaS = 1.0f)
	{
		SetSpatialSigma(sigmaS);
	};

	// Destructor.
	~GaussianFilter() {};

	// Update spatial sigma.
	void SetSpatialSigma(float sigmaS)
	{
		spatialSigma = sigmaS;
	}

	// Override virtual function for performing filtering.
	virtual cv::Mat Filter(const cv::Mat &img) override;

protected:
	// BoxFilter Protected Data.
	float spatialSigma;
};


// Class BilateralFilter Declaration.
class BilateralFilter : public GaussianFilter
{
public:
	// BilateralFilter Public Methods.

	// Constructor.
	BilateralFilter(const float sigmaS = 1.0f, const float sigmaR = 1.0f)
	{
		SetSpatialSigma(sigmaS);
		SetRangeSigma(sigmaR);
	};

	// Destructor.
	~BilateralFilter() {};

	// Update range sigma.
	void SetRangeSigma(float sigmaR)
	{
		rangeSigma = sigmaR;
	}

	// Override virtual function for performing filtering.
	virtual cv::Mat Filter(const cv::Mat &img) override;

private:
	// BilateralFilter Private Data.
	float rangeSigma;
};

#endif