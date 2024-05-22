#include <iostream>

#include <Windows.h>
#include <iostream>
#include "GUI.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

#include "ImageColorAdjuster.h"
#include "ImageScaler.h"
#include "ImageFilter.h"

using namespace std;


// Prototypes of methods.
cv::Mat GenerateImageBackground(cv::Size);
cv::Mat GenerateToolbarBackground(cv::Size);
string GetOpenImageFilePath();
void CreateUI();
void DrawBackgroundUI();
void OnLoadImage(cv::Mat&, cv::Mat&, int&, int&);
void OnResetImage(cv::Mat&, cv::Mat&, int&, int&);
void OnImageResize(cv::Mat&, cv::Mat&, int&, int&, const int, const int);
void OnRemappingLuminance(cv::Mat&, cv::Mat&);
void OnImageFiltering(cv::Mat&, cv::Mat&, float&, float&);


// Global variables for the window.
// This is not cool. However, I would like to focus on the image processing task.
cv::Mat windowFrame;
cv::Mat imgBackground;
cv::Mat toolbar;
cv::Mat normalLoadImgIcon, overLoadImgIcon;
cv::Mat normalResetImgIcon, overResetImgIcon;
cv::Mat normalGauFltIcon, overGauFltIcon;
cv::Mat normalBltFltIcon, overBltFltIcon;
cv::Mat colorCurveImg1, colorCurveImg2;
cv::Mat normalApplyImgIcon, overApplyImgIcon;
bool fixRatio = true;			// When resizing, should the aspect ratio be kept?
cv::Mat editImgOrigSize;		// To handle scaling from small to large image.


int main()
{
	// Create UI components and resources.
	CreateUI();

	// Init cvui and tell it to create a OpenCV window, i.e. cv::namedWindow(WINDOW_NAME).
    cvui::init(WINDOW_NAME);

	// The original (loaded image) and edited image (got scaled or filtered).
	// Please note the origImg might be scaled due to the UI's limitaion.
	cv::Mat origImg;
	cv::Mat editImg;
	
	// The current resolution (altered due to resizing). 
	int curWidth, curHeight;
	curWidth = curHeight = 0;
	
	// Default parameters for filtering.
	float sigmaS = 3.0f;
	float sigmaR = 15.0f;

	// Infinite loop for image editing.
    while (true) {
		// Record previous resolution (for recognizing if the width/height of the image has been changed). 
		int prevWidth  = curWidth;
		int prevHeight = curHeight; 
		
		// Render background to the frame
		DrawBackgroundUI();

		// Image scaling.
		OnLoadImage(origImg, editImg, curWidth, curHeight);

		// Image loading.
		OnResetImage(origImg, editImg, curWidth, curHeight);

		// Image resizing.
		OnImageResize(origImg, editImg, curWidth, curHeight, prevWidth, prevHeight);
		
		// Image color contrast adjustment.
		OnRemappingLuminance(origImg, editImg);

		// Image filtering.
		OnImageFiltering(origImg, editImg, sigmaS, sigmaR);
		
		
		// This function must be called *AFTER* all UI components. It does
		// all the behind the scenes magic to handle mouse clicks, etc.
		cvui::update();

		// Update cvui stuff and show everything on the screen
		cvui::imshow(WINDOW_NAME, windowFrame);
		cv::moveWindow(WINDOW_NAME, 10, 10);

		// Check whether the user exits the program.
		if (cv::waitKey(20) == ESC_KEY)
			break;
	}
	
    return 0;
}


// -----------------------------------------------------------------------------------------------
// The following methods handle stuffs for user interface.
// -----------------------------------------------------------------------------------------------
// Desc: Create UI components.
void CreateUI()
{
	// Create a frame where components will be rendered to.
	windowFrame = cv::Mat(cv::Size(WINDOW_WIDTH, WINDOW_HEIGHT), CV_8UC3);
	imgBackground = GenerateImageBackground(cv::Size(MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT));
	toolbar = GenerateToolbarBackground(cv::Size(TOOLBAR_WIDTH, TOOLBAR_HEIGHT));
	
	// Load images.
	normalLoadImgIcon = cv::imread("../Resources/load_icon.png", cv::IMREAD_COLOR);
	cv::resize(normalLoadImgIcon, normalLoadImgIcon, cv::Size(LOAD_BUTTON_SIZE, LOAD_BUTTON_SIZE));
	overLoadImgIcon = cv::imread("../Resources/load_icon_over.png", cv::IMREAD_COLOR);
	cv::resize(overLoadImgIcon, overLoadImgIcon, cv::Size(LOAD_BUTTON_SIZE, LOAD_BUTTON_SIZE));
	
	// Reset images.
	normalResetImgIcon = cv::imread("../Resources/reset_icon.png", cv::IMREAD_COLOR);
	cv::resize(normalResetImgIcon, normalResetImgIcon, cv::Size(RESET_BUTTON_SIZE_WIDTH, RESET_BUTTON_SIZE_HEIGHT));
	overResetImgIcon = cv::imread("../Resources/reset_icon_over.png", cv::IMREAD_COLOR);
	cv::resize(overResetImgIcon, overResetImgIcon, cv::Size(RESET_BUTTON_SIZE_WIDTH, RESET_BUTTON_SIZE_HEIGHT));

	// Color curve images.
	colorCurveImg1 = cv::imread("../Resources/color_curve_1.png", cv::IMREAD_COLOR);
	cv::resize(colorCurveImg1, colorCurveImg1, cv::Size(COLOR_CURVE_IMAGE_SIZE, COLOR_CURVE_IMAGE_SIZE));
	colorCurveImg2 = cv::imread("../Resources/color_curve_2.png", cv::IMREAD_COLOR);
	cv::resize(colorCurveImg2, colorCurveImg2, cv::Size(COLOR_CURVE_IMAGE_SIZE, COLOR_CURVE_IMAGE_SIZE));
	normalApplyImgIcon = cv::imread("../Resources/apply_icon.png", cv::IMREAD_COLOR);
	cv::resize(normalApplyImgIcon, normalApplyImgIcon, cv::Size(APPLY_BUTTON_SIZE_WIDTH, APPLY_BUTTON_SIZE_HEIGHT));
	overApplyImgIcon = cv::imread("../Resources/apply_over_icon.png", cv::IMREAD_COLOR);
	cv::resize(overApplyImgIcon, overApplyImgIcon, cv::Size(APPLY_BUTTON_SIZE_WIDTH, APPLY_BUTTON_SIZE_HEIGHT));

	// Filter button images.
	normalGauFltIcon = cv::imread("../Resources/gaussian_filter_icon.png", cv::IMREAD_COLOR);
	cv::resize(normalGauFltIcon, normalGauFltIcon, cv::Size(FILTER_BUTTON_SIZE_WIDTH, FILTER_BUTTON_SIZE_HEIGHT));
	overGauFltIcon = cv::imread("../Resources/gaussian_filter_icon_over.png", cv::IMREAD_COLOR);
	cv::resize(overGauFltIcon, overGauFltIcon, cv::Size(FILTER_BUTTON_SIZE_WIDTH, FILTER_BUTTON_SIZE_HEIGHT));
	
	normalBltFltIcon = cv::imread("../Resources/bilateral_filter_icon.png", cv::IMREAD_COLOR);
	cv::resize(normalBltFltIcon, normalBltFltIcon, cv::Size(FILTER_BUTTON_SIZE_WIDTH, FILTER_BUTTON_SIZE_HEIGHT));
	overBltFltIcon = cv::imread("../Resources/bilateral_filter_icon_over.png", cv::IMREAD_COLOR);
	cv::resize(overBltFltIcon, overBltFltIcon, cv::Size(FILTER_BUTTON_SIZE_WIDTH, FILTER_BUTTON_SIZE_HEIGHT));
}

// Desc: Draw background UI components.
void DrawBackgroundUI()
{
	cvui::image(windowFrame, 0, 0, imgBackground);
	cvui::image(windowFrame, 0, MAX_IMAGE_HEIGHT, toolbar);
}


// Desc: Handle image loading.
void OnLoadImage(cv::Mat& origImg, cv::Mat& editImg, int& curWidth, int& curHeight)
{
	// Load new image.
	cvui::text(windowFrame, LOAD_TEXT_POS_X, LOAD_TEXT_POS_Y, LOAD_TEXT);
	if (cvui::button(windowFrame, LOAD_BUTTON_POS_X, LOAD_BUTTON_POS_Y, normalLoadImgIcon, overLoadImgIcon, normalLoadImgIcon)) {
		string filename = GetOpenImageFilePath();
		if (filename != "") {
			origImg = cv::imread(filename, cv::IMREAD_COLOR);
			int w = origImg.cols;
			int h = origImg.rows;
			// Resize the image if the image size is not in the range.
			if (w >= h) {
				if (w > MAX_IMAGE_WIDTH) {
					float ratio = (float)w / (float)h;
					w = MAX_IMAGE_WIDTH;
					h = (int)round(w / ratio);
				}
				if (h < MIN_IMAGE_HEIGHT) {
					float ratio = (float)w / (float)h;
					h = MIN_IMAGE_HEIGHT;
					w = (int)round(ratio * h);
				}
			}
			else {
				if (h > MAX_IMAGE_HEIGHT) {
					float ratio = (float)w / (float)h;
					h = MAX_IMAGE_HEIGHT;
					w = (int)round(ratio * h);
				}
				if (w < MIN_IMAGE_WIDTH) {
					float ratio = (float)w / (float)h;
					w = MIN_IMAGE_WIDTH;
					h = (int)round(w / ratio);
				}
			}
			// Use OpenCV's built-in function to resize.
			cv::resize(origImg, origImg, cv::Size(w, h));
			// Making copy to editImg.
			editImg = origImg.clone();
			editImgOrigSize = origImg.clone();
			curWidth  = w;
			curHeight = h;
		}
	}
}


// Desc: Handle image reset.
void OnResetImage(cv::Mat& origImg, cv::Mat& editImg, int &curWidth, int& curHeight)
{
	if (!origImg.empty()) {
		// Reset.
		if (cvui::button(windowFrame, RESET_BUTTON_POS_X, RESET_BUTTON_POS_Y, normalResetImgIcon, overResetImgIcon, normalResetImgIcon)) {
			editImg.release();
			curWidth = origImg.cols;
			curHeight = origImg.rows;
			editImg = origImg.clone();
			editImgOrigSize = origImg.clone();
		}
	}
}

// Desc: Handle image resizing.
void OnImageResize(cv::Mat& origImg, cv::Mat& editImg, int &curWidth, int& curHeight, const int prevWidth, const int prevHeight)
{
	// If input image has not been loaded, do nothing.
	if (!origImg.empty()) {
		// Adjust resolution.
		cvui::text(windowFrame, RES_TEXT_POS_X, RES_TEXT_POS_Y, RES_TEXT);
		cvui::text(windowFrame, RES_WIDTH_TEXT_POS_X, RES_WIDTH_TEXT_POS_Y, RES_WIDTH_TEXT);
		cvui::trackbar(windowFrame, RES_WIDTH_TRACKBAR_POS_X, RES_WIDTH_TRACKBAR_POS_Y, RES_TRACKBER_SIZE, &curWidth, MIN_IMAGE_WIDTH, MAX_IMAGE_WIDTH);
		cvui::text(windowFrame, RES_HEIGHT_TEXT_POS_X, RES_HEIGHT_TEXT_POS_Y, RES_HEIGHT_TEXT);
		cvui::trackbar(windowFrame, RES_HEIGHT_TRACKBAR_POS_X, RES_HEIGHT_TRACKBAR_POS_Y, RES_TRACKBER_SIZE, &curHeight, MIN_IMAGE_HEIGHT, MAX_IMAGE_HEIGHT);
		cvui::checkbox(windowFrame, RES_FIXEDRATIO_CHECKBOX_TEXT_POS_X, RES_FIXEDRATIO_CHECKBOX_TEXT_POS_Y, RES_FIXEDRATIO_CHECKBOX_TEXT, &fixRatio);
		if (fixRatio) {
			// If image's aspect ratio needs to be kept. 
			float ratio = (float)origImg.cols / (float)origImg.rows;
			if (prevWidth != curWidth) {
				// Image width has been adjusted, update image height.
				curHeight = (int)round(curWidth / ratio);
				// Need to make sure the new height is valid.
				if (curHeight > MAX_IMAGE_HEIGHT) {
					curHeight = MAX_IMAGE_HEIGHT;
					curWidth = (int)round(ratio * curHeight);
				}
				else if (curHeight < MIN_IMAGE_HEIGHT) {
					curHeight = MIN_IMAGE_HEIGHT;
					curWidth = (int)round(ratio * curHeight);
				}
			}
			if (prevHeight != curHeight) {
				// Image height has been adjusted, update image width.
				curWidth = (int)round(ratio * curHeight);
				// Need to make sure the new width is valid.
				if (curWidth > MAX_IMAGE_WIDTH) {
					curWidth  = MAX_IMAGE_WIDTH;
				 	curHeight = (int)round(curWidth / ratio);
				}
				else if (curWidth < MIN_IMAGE_WIDTH) {
					curWidth  = MIN_IMAGE_WIDTH;
				 	curHeight = (int)round(curWidth / ratio);
				}
			}
		}

		// Scale the image to the new target resolution.
		if (prevWidth != curWidth || prevHeight != curHeight)
			editImg = ImageScaler::Resize(editImgOrigSize, curWidth, curHeight);

		// Show the edited image at the center of image canvas.
		int startX = MAX_IMAGE_WIDTH  / 2 - curWidth / 2;
		int startY = MAX_IMAGE_HEIGHT / 2 - curHeight / 2;
		cvui::image(windowFrame, startX, startY, editImg);
	}
}


// Desc: Handle image luminance remapping.
void OnRemappingLuminance(cv::Mat& origImg, cv::Mat& editImg)
{
	if (!editImg.empty()) {
		cvui::text(windowFrame, COLOR_CONTRAST_TEXT_POS_X, COLOR_CONTRAST_TEXT_POS_Y, COLOR_CONTRAST_TEXT);
		
		cvui::image(windowFrame, COLOR_CURVE_IMG_1_POS_X, COLOR_CURVE_IMG_1_POS_Y, colorCurveImg1);
		if (cvui::button(windowFrame, APPLY_BUTTON_1_POS_X, APPLY_BUTTON_1_POS_Y, normalApplyImgIcon, overApplyImgIcon, normalApplyImgIcon)) {
			editImg = ImageColorAdjuster::RemapColorCurve1(editImg);
			editImgOrigSize = ImageColorAdjuster::RemapColorCurve1(editImgOrigSize);
		}
		
		cvui::image(windowFrame, COLOR_CURVE_IMG_2_POS_X, COLOR_CURVE_IMG_2_POS_Y, colorCurveImg2);
		if (cvui::button(windowFrame, APPLY_BUTTON_2_POS_X, APPLY_BUTTON_2_POS_Y, normalApplyImgIcon, overApplyImgIcon, normalApplyImgIcon)) {
			editImg = ImageColorAdjuster::RemapColorCurve2(editImg);
			editImgOrigSize = ImageColorAdjuster::RemapColorCurve2(editImgOrigSize);
		}
	}
}


// Desc: Handle image filtering.
void OnImageFiltering(cv::Mat& origImg, cv::Mat& editImg, float& sigmaS, float& sigmaR)
{
	if (!origImg.empty()) {
		cvui::text(windowFrame, FILTER_TEXT_POS_X, FILTER_TEXT_POS_Y, FILTER_TEXT);
		
		cvui::text(windowFrame, FILTER_SIGMAS_TEXT_POS_X, FILTER_SIGMAS_TEXT_POS_Y, FILTER_SIGMAS_TEXT);
		cvui::trackbar(windowFrame, FILTER_SIGMAS_TRACKBAR_POS_X, FILTER_SIGMAS_TRACKBAR_POS_Y, FILTER_TRACKBAR_SIZE, &sigmaS, MIN_FILTER_SIGMA_S, MAX_FILTER_SIGMA_S);
		cvui::text(windowFrame, FILTER_SIGMAR_TEXT_POS_X, FILTER_SIGMAR_TEXT_POS_Y, FILTER_SIGMAR_TEXT);
		cvui::trackbar(windowFrame, FILTER_SIGMAR_TRACKBAR_POS_X, FILTER_SIGMAR_TRACKBAR_POS_Y, FILTER_TRACKBAR_SIZE, &sigmaR, MIN_FILTER_SIGMA_R, MAX_FILTER_SIGMA_R);
		
		// Perform Gaussian filtering.
		if (cvui::button(windowFrame, GAUSSIAN_FILTER_BUTTON_POS_X, GAUSSIAN_FILTER_BUTTON_POS_Y, normalGauFltIcon, overGauFltIcon, normalGauFltIcon)) {
			GaussianFilter gaussianFilter(sigmaS);
			editImg = gaussianFilter.Filter(editImg);
			editImgOrigSize = gaussianFilter.Filter(editImgOrigSize);
		}
				
		// Perform bilateral filtering.
		if (cvui::button(windowFrame, BILATERAL_FILTER_BUTTON_POS_X, BILATERAL_FILTER_BUTTON_POS_Y, normalBltFltIcon, overBltFltIcon, normalBltFltIcon)) {
			BilateralFilter bilateralFilter(sigmaS, sigmaR);
			editImg = bilateralFilter.Filter(editImg);
			editImgOrigSize = bilateralFilter.Filter(editImgOrigSize);
		}
	}
}


// Desc: Generate a checkboard pattern background panel.
cv::Mat GenerateImageBackground(cv::Size size)
{
	cv::Mat bg = cv::Mat(size, CV_8UC3);
	
	int nGridX = (int)ceil((float)MAX_IMAGE_WIDTH  / GRID_SIZE);
	int nGridY = (int)ceil((float)MAX_IMAGE_HEIGHT / GRID_SIZE);
	cv::Size gridSize = cv::Size(GRID_SIZE, GRID_SIZE);
	
	cv::Vec3b lightGridColor = cv::Vec3b(LIGHT_GRID_COLOR, LIGHT_GRID_COLOR, LIGHT_GRID_COLOR);
	cv::Vec3b darkGridColor  = cv::Vec3b(DARK_GRID_COLOR,  DARK_GRID_COLOR,  DARK_GRID_COLOR );

	for (int y = 0; y < nGridY; ++y) {
		bool even = (y % 2 == 0);
		for (int x = 0; x < nGridX; ++x) {
			if (even) {
				if (x % 2 == 0)
					cv::rectangle(bg, cv::Rect(cv::Point(x * GRID_SIZE, y * GRID_SIZE), gridSize), lightGridColor, -1);
				else
					cv::rectangle(bg, cv::Rect(cv::Point(x * GRID_SIZE, y * GRID_SIZE), gridSize), darkGridColor, -1);
			}
			else {
				if (x % 2 == 1)
					cv::rectangle(bg, cv::Rect(cv::Point(x * GRID_SIZE, y * GRID_SIZE), gridSize), lightGridColor, -1);
				else
					cv::rectangle(bg, cv::Rect(cv::Point(x * GRID_SIZE, y * GRID_SIZE), gridSize), darkGridColor, -1);
			}
		}
	}
	return bg;
}

// Desc: Generate a constant color toolbar panel.
cv::Mat GenerateToolbarBackground(cv::Size size)
{
	const cv::Vec3b toolbarColor = cv::Vec3b(TOOLBAR_COLOR, TOOLBAR_COLOR, TOOLBAR_COLOR);
	cv::Mat toolbar = cv::Mat(size, CV_8UC3);
	toolbar = toolbarColor;
	return toolbar;
}

// Desc: Fire a windows style dialog for selecting file (copied from Internet, might have bug ...).
string GetOpenImageFilePath()
{
    OPENFILENAME ofn;							// Common dialog box structure.
    wchar_t szFile[MAX_STRING_BUFFER_LENGTH];   // Buffer for file name.
    HWND hwnd;									// Owner window.
    HANDLE hf;									// File handle.

    hwnd = GetForegroundWindow();

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    // Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
    // use the contents of szFile to initialize itself.
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Image Files\0*.png;*.jpg;*.jpeg\0 \0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn) == TRUE) {
        std::wstring ws(szFile);
        std::string str(ws.begin(), ws.end());
        return str;
    }
    return "";
}