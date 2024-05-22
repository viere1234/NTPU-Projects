#ifndef VECTOR_TOOLS_H
#define VECTOR_TOOLS_H

#include "opencv2/opencv.hpp"

// Since we use C++14, we custom a clamp function.
template<typename T>
static inline T clamp(const T& value, const T& minValue, const T& maxValue) {
	return value < minValue ? minValue : value > maxValue ? maxValue : value;
}

template<typename T>
static inline cv::Vec<T, 2> clampVec2(const cv::Vec<T, 2>& vec, const T& minValue, const T& maxValue) {
	return cv::Vec<T, 2>(
		clamp(vec[0], minValue, maxValue),
		clamp(vec[1], minValue, maxValue)
	);
}

template<typename T>
static inline cv::Vec<T, 3> clampVec3(const cv::Vec<T, 3>& vec, const T& minValue, const T& maxValue) {
	return cv::Vec<T, 3>(
		clamp(vec[0], minValue, maxValue),
		clamp(vec[1], minValue, maxValue),
		clamp(vec[2], minValue, maxValue)
	);
}

#endif
