#include"LightDescriptor.h"

LightDescriptor::LightDescriptor(const cv::RotatedRect& light)
{
    width = light.size.width;
    length = light.size.height;
    center = light.center;
    angle = light.angle;
    area = light.size.area();
}
