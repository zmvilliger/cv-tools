#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H
#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cstdint>
#include <map>


struct ImageFilter {
	cv::Mat img;
	cv::VideoCapture cap;

	std::set<std::string> effectSet;

	std::vector<cv::Mat> channelsHSV;
	std::vector<cv::Mat> channelsBGR;

	// Checkbox booleans to enable openCV image filters
	bool cutBlue = false;
	bool cutGreen = false;
	bool cutRed = false;
	bool cutWhite = false;
	bool isoGreen = false;

	bool edges = false;
	bool blur = false;
	bool hsv = false;
	bool isoHue = false;

	bool clear = false;

	// Lower value for thresholding of color channels
	int blueLowerThreshold;
	int greenLowerThreshold;
	int redLowerThreshold;
	int whiteLowerThreshold;

	// Upper value for thresholding of color channels
	int blueUpperThreshold;
	int greenUpperThreshold;
	int redUpperThreshold;
	int whiteUpperThreshold;

	// Effect params
	int edgesUpperThreshold;
	int edgesLowerThreshold;

	int blurKernelX;
	int blurKernelY;

	int hueShift;
	int isoHueLower;
	int isoHueUpper;

	std::vector<std::function<void(cv::Mat&, int, int)>> filterFunctionOrder;
	std::map<std::string, std::function<void(cv::Mat&, int, int)>> filterFunctionMap;

};

ImageFilter init(cv::VideoCapture cap);

void update(ImageFilter& filter);



#endif
