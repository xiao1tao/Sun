#include<opencv2/opencv.hpp>
#include<iostream>
using namespace std;
using namespace cv;
#include"LightDescriptor.h"

int main()
{
    VideoCapture capture("D:/HONOR Magic-link/Screenshot/jjj.mp4");
    
    while (true)
    {
        Mat frame;
        capture >> frame;
        if (frame.empty())
        {
            break;
        }


 //图像预处理
        vector<Mat>channels;
        split(frame, channels);
        Mat red = channels.at(2);
        threshold(red, red, 210, 255, THRESH_BINARY);//inRange()可实现二值化（与threshold(）类似）
        Canny(red, red, 0, 30, 3);
        GaussianBlur(red, red, Size(3, 3), 0);
        Mat element = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
        dilate(red, red, element);
        morphologyEx(red, red, MORPH_CLOSE, 0);
        vector<vector<Point>>contours;
        vector<Vec4i>hierachy;
        findContours(red, contours, hierachy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        

 //拟合部分
        vector<LightDescriptor> lightInfos;//创建一个灯条类的动态数组
        for (int i = 0; i < contours.size(); i++)
        {
            double area = contourArea(contours[i]);
            
            // 去除较小轮廓&fitEllipse的限制条件
            if (area < 5 || contours[i].size() <= 1)
                continue;
            
            // 用椭圆拟合区域得到外接矩形
            RotatedRect Light_Rec = fitEllipse(contours[i]);

            // 长宽比和轮廓面积比限制
            if (Light_Rec.size.width / Light_Rec.size.height > 4)
                continue;
            
            // 扩大灯柱的面积
            Light_Rec.size.height *= 1.2;
            Light_Rec.size.width *= 1.2;
            
            lightInfos.push_back(LightDescriptor(Light_Rec));
        }
        

        //lightInfos是返回的被筛选好的灯条
        //二重循环多条件匹配灯条（在原先灯条的基础上去匹配剩下的灯条）
        for (size_t i = 0; i < lightInfos.size(); i++) 
        {
            for (size_t j = i + 1; j < lightInfos.size(); j++) 
            {
                LightDescriptor& leftLight = lightInfos[i];
                LightDescriptor& rightLight = lightInfos[j];

                //角差
                float angleDiff_ = abs(leftLight.angle - rightLight.angle);//abs()函数，求绝对值
               
               //// //长度差比率（由于远近的关系，需要考虑的应该是个差距的比值而不是差距本身
                float LenDiff_ratio = abs(leftLight.length - rightLight.length) / max(leftLight.length, rightLight.length);
               
                //筛选
                if (angleDiff_ > 10 || LenDiff_ratio > 0.8) 
                {
                    continue;
                }

                //左右灯条相距距离      pow(float x,,float y);计算x的y次幂
                float dis = pow(pow((leftLight.center.x - rightLight.center.x), 2) + pow((leftLight.center.y - rightLight.center.y), 2), 0.5);
                //左右灯条长度的平均值
                float meanLen = (leftLight.length + rightLight.length) / 2;
                //左右灯条长度差比值
                float lendiff_datio = abs(leftLight.length - rightLight.length) / meanLen;
                //左右灯条中心点y的差值
                float yDiff = abs(leftLight.center.y - rightLight.center.y);
                //y差比率
                float yDiff_ratio = yDiff / meanLen;
                //左右灯条中心点x的差值
                float xDiff = abs(leftLight.center.x - rightLight.center.x);
                //x差比率
                float xDiff_ratio = xDiff / meanLen;
                //相距距离与灯条长度比值
                float ratio = dis / meanLen;
                
                //筛选,匹配不通过的条件
                if (angleDiff_>15||
                    LenDiff_ratio>1.0||
                    lendiff_datio>0.8||
                    yDiff_ratio>1.5||
                    xDiff_ratio>2.2||
                    xDiff_ratio<0.8||
                    ratio>3||
                    ratio<0.8)
                {  continue;
                }
               
                //绘制矩形
                Point center = Point((leftLight.center.x + rightLight.center.x) / 2, (leftLight.center.y + rightLight.center.y) / 2);
                RotatedRect rect = RotatedRect(center, Size(dis, meanLen), (leftLight.angle + rightLight.angle) / 2);
                Point2f vertices[4];
                rect.points(vertices);
               
                for (int i = 0; i < 4; i++)
                {
                    line(frame, vertices[i], vertices[(i + 1) % 4], Scalar(0, 0, 255), 2);
                  /*  putText(frame, format("%d",2), center, 2, 2, Scalar(0, 255, 0));*/
                }
               /* for (int i = 0; i < 4; i++)
                {
                    if (i == 3)
                    {
                        line(frame, vertices[i], vertices[0], Scalar(0, 0, 255), 2);
                    }
                    line(frame, vertices[i], vertices[i + 1], Scalar(0, 0,255), 2);
                }*/


            }
        }

        /*imshow("装甲板识别1", red);*/
        imshow("装甲板识别", frame);
        waitKey(5);
        

    }
    
    capture.release();
    cv::destroyAllWindows();
    return 0;

}
