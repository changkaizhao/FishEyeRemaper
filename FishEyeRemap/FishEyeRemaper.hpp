//
//  FishEyeRemaper.hpp
//  FishEyeRemap
//
//  Created by 赵常凯 on 16/6/21.
//  Copyright © 2016年 赵常凯. All rights reserved.
//

#ifndef FishEyeRemaper_hpp
#define FishEyeRemaper_hpp

#include <stdio.h>
#include <opencv2/opencv.hpp>
class FishEyeRemaper{
public:
    double apenture;  //(degree) the max angle of fisheye camera len
    unsigned int outputsize; // output image height ,width is proportional to height
    float center_x;    //principle axis at center x coordinate of CMOS
    float center_y;
    float pixelsize;  //cmos pixel size
    cv::Size CMOS_size; //cmos size
    double (&fitfunc)[10];  //function of theta(intent ray angle) to r(radius to center)
    inline double getRadius(double degree){
        return (this->fitfunc[0]*degree*degree*degree*degree*degree*degree*degree*degree*degree+
                this->fitfunc[1]*degree*degree*degree*degree*degree*degree*degree*degree+
                this->fitfunc[2]*degree*degree*degree*degree*degree*degree*degree+
                this->fitfunc[3]*degree*degree*degree*degree*degree*degree+
                this->fitfunc[4]*degree*degree*degree*degree*degree+
                this->fitfunc[5]*degree*degree*degree*degree+
                this->fitfunc[6]*degree*degree*degree+
                this->fitfunc[7]*degree*degree+
                this->fitfunc[8]*degree+
                this->fitfunc[9]);};
    


    FishEyeRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10]):CMOS_size(CMOS_size),pixelsize(pixelsize),outputsize(outputsize),fitfunc(fitfunc),apenture(180),center_x(CMOS_size.width/2.0),center_y(CMOS_size.height/2.0){/*compute();*/};
    
    
    FishEyeRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture):CMOS_size(CMOS_size),pixelsize(pixelsize),outputsize(outputsize),fitfunc(fitfunc),apenture(apenture),center_x(CMOS_size.width/2.0),center_y(CMOS_size.height/2.0){/*compute();*/};
    
    
    FishEyeRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture,unsigned int center_x,unsigned int center_y):CMOS_size(CMOS_size),pixelsize(pixelsize),outputsize(outputsize),fitfunc(fitfunc),apenture(apenture),center_x(center_x),center_y(center_y){/*compute();*/};
    
    // should be inplement by child class
    virtual void compute() = 0;
    //void reset(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture,float center_x,float center_y);
    cv::Mat remap(const cv::Mat inputImg);  //remap fisheye img to flat img
   // static cv::Mat remap(const cv::Mat inputImg,const cv::Mat remattable);
    virtual ~FishEyeRemaper();
    
    cv::Mat remaptable;
};

class FishEyeSphereRemaper : public FishEyeRemaper{
public:
    FishEyeSphereRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10]):FishEyeRemaper(CMOS_size,pixelsize,outputsize,fitfunc,180,CMOS_size.width/2.0,CMOS_size.height/2.0){compute();};
    FishEyeSphereRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture):FishEyeRemaper(CMOS_size,pixelsize,outputsize,fitfunc,apenture,CMOS_size.width/2.0,CMOS_size.height/2.0){compute();};
    FishEyeSphereRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture,unsigned int center_x,unsigned int center_y):FishEyeRemaper(CMOS_size,pixelsize,outputsize,fitfunc,apenture,center_x,center_y){compute();};
    void compute();
     ~FishEyeSphereRemaper();
    //cv::Mat remap(const cv::Mat inputImg);  //remap fisheye img to flat img

private:
    void initflatindextable();
};
class FishEyeRectRemaper : public FishEyeRemaper{
public:
    FishEyeRectRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10]):FishEyeRemaper(CMOS_size,pixelsize,outputsize,fitfunc,180,CMOS_size.width/2.0,CMOS_size.height/2.0){compute();};
    FishEyeRectRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture):FishEyeRemaper(CMOS_size,pixelsize,outputsize,fitfunc,apenture,CMOS_size.width/2.0,CMOS_size.height/2.0){compute();};
    FishEyeRectRemaper(const cv::Size CMOS_size,float pixelsize,unsigned int outputsize,double (&fitfunc)[10],double apenture,unsigned int center_x,unsigned int center_y):FishEyeRemaper(CMOS_size,pixelsize,outputsize,fitfunc,apenture,center_x,center_y){compute();};
    void compute();
    ~FishEyeRectRemaper();
    //cv::Mat remap(const cv::Mat inputImg);  //remap fisheye img to flat img
    
private:
    void initflatindextable();
};
#endif /* FishEyeRemaper_hpp */
