//
//  main.cpp
//  OpencvMAC
//
//  Created by 赵常凯 on 14-8-22.
//  Copyright (c) 2014年 赵常凯. All rights reserved.
//

#include <iostream>
#include <opencv2/opencv.hpp>
#include "FishEyeRemaper.hpp"
#define infinite 1e400
using namespace std;
using namespace cv;
const string rootPath = "/Development/Github/FishEyeRemaper";
const string  filename = rootPath+"/IMG_2513.JPG";//spherical0.jpg";

double fitfunc[10] = {0,0,0,-4.992504382633377e-13,5.766845887936866e-11,-1.507167625543759e-9,4.922922345681327e-07,1.005862302995363e-05,0.014342794415958,4.080769765804198e-04}; //240 degree len
double fitfunc2[10] = {0,0,0,1.741445308576458e-13,-1.023407293784365e-10,7.508696701055800e-9,-4.333280169563893e-7,5.480537036722254e-06,0.022779016868135,5.699063505086269e-05}; //180 degree len
float pixelsize = 1.4213;//6.5689789 ; // um      for real one 2.2um
cv::Size cmossize = cv::Size(3264,2448);

int main(int argc, const char * argv[])
{
    cv::Mat fishimg = cv::imread(filename);
    //FishEyeSphereRemaper fer(cmossize,pixelsize,fishimg.rows,fitfunc2,180,1591.7,1223.93); //sphere transform
    FishEyeRectRemaper fer(cmossize,pixelsize,fishimg.rows,fitfunc2,180,1591.7,1223.93);
    cv::Mat output = fer.remap(fishimg);
    cv::imshow("fishimg", fishimg);
    cv::imshow("output", output);
    
    
    
//    cv::Mat test = (cv::Mat_<double>(3,3)<<1,2,3,4,5,6,7,8,9);
//    printf("\n%f---_%d",test.at<double>(2,1),(int)(-533/2.0));
    cv::imwrite(rootPath+"output.jpg", output);
    cv::waitKey(0);

    return 0;
}
