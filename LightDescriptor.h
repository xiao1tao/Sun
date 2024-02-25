#pragma once
#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;

class LightDescriptor
{
public:
    LightDescriptor();
    LightDescriptor(const cv::RotatedRect& light);
    
public:
    float width;
    float length;
    Point2f center;
    float angle;
    float area;
};
