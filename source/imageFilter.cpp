#include "imageFilter.h"
#include <cstdint>


cv::Mat blur(cv::Mat img) {
    cv::blur(img, img, cv::Size(5, 5));
    return img;
}

cv::Mat edges(cv::Mat img) {

    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
    cv::Canny(img, img, 100, 200);

        return img;
}

ImageFilter init(cv::VideoCapture cap) {

    ImageFilter filter;

    std::set<std::string> effectSet;

    filter.cap = cap;

    filter.cap.read(filter.img);

    filter.cutBlue = false;
    filter.cutGreen = false;
    filter.cutRed = false;
    filter.cutWhite = false;

    filter.edges = false;
    filter.blur = false;
    filter.hsv = false;

    // Lower value for thresholding of color channels
    filter.blueLowerThreshold = 100;
    filter.greenLowerThreshold = 100;
    filter.redLowerThreshold = 100;
    filter.whiteLowerThreshold = 100;
    
    // Value for thresholding of color channels
    filter.blueUpperThreshold = 200;
    filter.greenUpperThreshold = 200;
    filter.redUpperThreshold = 200;
    filter.whiteUpperThreshold = 200;

    // Upper value for thresholding of color channels
    filter.blueUpperThreshold = 100;
    filter.greenUpperThreshold = 100;
    filter.redUpperThreshold = 100;
    filter.whiteUpperThreshold = 100;

    // Effect params
    filter.edgesUpperThreshold = 200;
    filter.edgesLowerThreshold = 100;
    filter.blurKernelX = 5;
    filter.blurKernelY = 5;

    filter.hueShift = 0;

    return filter;

}

void update(ImageFilter& filter) {

    
    int low = 5;
    int upp = 5;

    if (filter.clear) {
        filter.filterFunctionOrder.clear();
        filter.effectSet.clear();
        return;
    }


    ///////////////////// Lambdas for image effect queueing ///////////////////////////

    filter.filterFunctionMap["blur"] = [&filter](cv::Mat& img, int sizeX, int sizeY) -> void {
        cv::blur(filter.img, filter.img, cv::Size(filter.blurKernelX, filter.blurKernelY));  // TODO MAKE BLUR SIZE ADJUSTABLE
        };

    filter.filterFunctionMap["edges"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::Mat resultImg;
        cv::Canny(filter.img, resultImg, filter.edgesLowerThreshold, filter.edgesUpperThreshold); // TODO MAKE CANNY THRESHOLDS ADJUSTABLE
        resultImg.copyTo(img);
        };

    filter.filterFunctionMap["cutWhite"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::threshold(filter.img, filter.img, filter.whiteLowerThreshold, filter.whiteUpperThreshold, cv::THRESH_TOZERO_INV);
        };

    filter.filterFunctionMap["cutBlue"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::threshold(filter.img, filter.img, filter.blueLowerThreshold, filter.blueUpperThreshold, cv::THRESH_TOZERO_INV);
        };

    filter.filterFunctionMap["isoGreen"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::threshold(filter.img, filter.img, filter.greenLowerThreshold, filter.greenUpperThreshold, cv::THRESH_TOZERO);
        };

    filter.filterFunctionMap["cutRed"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::threshold(filter.img, filter.img, filter.redLowerThreshold, filter.redUpperThreshold, cv::THRESH_TOZERO_INV);
        };

    filter.filterFunctionMap["isoHue"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::Mat tmp;
        cv::Mat mask;
        cv::Mat resultImg;

        cv::cvtColor(filter.img, tmp, cv::COLOR_BGR2HSV);
        cv::inRange(tmp, cv::Scalar(filter.isoHueLower, 0, 0),
                              cv::Scalar(filter.isoHueUpper, 255, 255), mask);
        cv::bitwise_and(filter.img, filter.img, resultImg, mask);

        filter.img = resultImg;
        };

    filter.filterFunctionMap["hueShift"] = [&filter](cv::Mat& img, int lower, int upper) -> void {
        cv::Mat tmp;
        cv::cvtColor(filter.img, tmp, cv::COLOR_BGR2HSV);
        std::vector<cv::Mat> hsvChannels;
        cv::split(tmp, hsvChannels);
        cv::add(hsvChannels[0], filter.hueShift, hsvChannels[0]);
        cv::merge(hsvChannels, filter.img);
        };


    ///////////////////////////////////////////////////////////////////////////////////

    std::vector<std::function<void(cv::Mat&, int, int)>> filterFunctionOrder = filter.filterFunctionOrder;
    std::map<std::string, std::function<void(cv::Mat&, int, int)>> filterFunctionMap = filter.filterFunctionMap;


    cv::Mat img = filter.img;
    if (filter.blur && (filter.effectSet.find("blur") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["blur"]);
        filter.effectSet.insert("blur");
    }

    if (filter.edges && (filter.effectSet.find("edges") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["edges"]);
        filter.effectSet.insert("edges");
    }

    if (filter.hueShift != 0 && (filter.effectSet.find("hueShift") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["hueShift"]);
        filter.effectSet.insert("hueShift");
    }

    if (filter.cutWhite && (filter.effectSet.find("cutWhite") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["cutWhite"]);
        filter.effectSet.insert("cutWhite");
    }

    if (filter.cutBlue && (filter.effectSet.find("cutBlue") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["cutBlue"]);
        filter.effectSet.insert("cutBlue");
    }

    if (filter.isoGreen && (filter.effectSet.find("isoGreen") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["isoGreen"]);
        filter.effectSet.insert("isoGreen");
    }

    if (filter.cutRed && (filter.effectSet.find("cutRed") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["cutRed"]);
        filter.effectSet.insert("cutRed");
    }
    if (filter.isoHue && (filter.effectSet.find("isoHue") == filter.effectSet.end())) {
        filter.filterFunctionOrder.push_back(filter.filterFunctionMap["isoHue"]);
        filter.effectSet.insert("isoHue");
    }
    for (std::function<void(cv::Mat&, int, int)> effect : filterFunctionOrder) {
        effect(filter.img, low, upp);
        std::cout << " effect applied " << std::endl;
    }

    std::cout << "function vector size: " << filterFunctionOrder.size() << std::endl;
    std::cout << "lambda map sz: " << filterFunctionMap.size() << std::endl;

}