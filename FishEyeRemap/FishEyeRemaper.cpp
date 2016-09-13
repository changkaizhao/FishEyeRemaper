//
//  FishEyeRemaper.cpp
//  FishEyeRemap
//
//  Created by 赵常凯 on 16/6/21.
//  Copyright © 2016年 赵常凯. All rights reserved.
//

#include "FishEyeRemaper.hpp"



cv::Mat flattable; //save longitude and latitude
cv::Mat flatindex; //save x,y coordinates in fisheye image
cv::Mat factorindex; //save i,j of factorTable

unsigned int Output_Width;
unsigned int Output_Height;

//factor table a,b,c,d  by 32
//-+----+----+-
// |  a |  b |
//-+----+----+-
// |  c |  d |
//-+----+----+-

cv::Mat factorTable;
cv::Mat indextable;
void initfactorTable(){
    factorTable = cv::Mat(32,32,CV_32FC4);
    
//    double t = (double)cv::getTickCount();
//    if (factorTable.isContinuous()) {
//        float *pdata = (float*)factorTable.data;
//        int index = 0;
//        for (int i = 0; i < 4096; i+=4) {
//            
//            float aa = (float)(index % 32)/31.0;
//            float cc = 1.0 - aa;
//            float bb = floorf((float)index / 32.0)/31.0;
//            float dd = 1.0 - bb;
//    
//            float *pt = &pdata[i];
//            pt[0] = cc * dd;
//            pt[1] = aa * dd;
//            pt[2] = cc * bb;
//            pt[3] = aa * bb;
//            index++;
//        }
//    }
//    t = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
//    
//    printf("time is %f ms\n",t*1000);
//    double tt = (double)cv::getTickCount();
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32 ; j++) {
            //at.<>(x,y)  y = row x = col
            float aa = (float)i/31.0;
            float cc = 1.0 - aa;
            float bb = (float)j/31.0;
            float dd = 1.0 - bb;
            factorTable.at<cv::Vec4f>(j,i)[0] = cc * dd;
            factorTable.at<cv::Vec4f>(j,i)[1] = aa * dd;
            factorTable.at<cv::Vec4f>(j,i)[2] = cc * bb;
            factorTable.at<cv::Vec4f>(j,i)[3] = aa * bb;
        }
    }

}


void logd(){
    for (int i = 0; i<Output_Height; i++) {
        for(int j = 0; j<Output_Width;j++){
            printf(" [%.2d,%.2d] ",flatindex.at<cv::Vec2w>(i,j)[0],flatindex.at<cv::Vec2w>(i,j)[1]);
        }
        printf("\n");
    }
}
void logf(){
    for (int i = 0; i<Output_Height; i++) {
        for(int j = 0; j<Output_Width;j++){
            printf(" [%.2f,%.2f] ",flattable.at<cv::Vec2f>(i,j)[0],flattable.at<cv::Vec2f>(i,j)[1]);
        }
        printf("\n");
    }
}



void initflatTable(double apenture,unsigned int outputsize){
    Output_Width = (apenture/180)*outputsize;
    Output_Height = outputsize;
    double stepX = apenture/(float)(Output_Width-1);
    double stepY = 180.0/(float)Output_Height;
    
    
    //inner area
    flattable = cv::Mat(Output_Height,Output_Width,CV_32FC2);
    for (int i = 1; i < Output_Width-1; i++) {
        for(int j = 1 ;j < Output_Height-1;j++){
            flattable.at<cv::Vec2f>(j,i)[0] = -apenture/2.0 + stepX * i;
            flattable.at<cv::Vec2f>(j,i)[1] = 90 - stepY * j;
        }
    }
    
    
    //top and bottom row
    for (int i = 0; i < Output_Width;i++) {
        flattable.at<cv::Vec2f>(0,i)[0] = -apenture/2.0f + stepX*i;
        flattable.at<cv::Vec2f>(0,i)[1] = 90;
        
        flattable.at<cv::Vec2f>(Output_Height-1,i)[0] = -apenture/2.0f + stepX*i;
        flattable.at<cv::Vec2f>(Output_Height-1,i)[1] = 90;
        
    }
    flattable.at<cv::Vec2f>(0,Output_Width-1)[0] = apenture/2.0f;
    flattable.at<cv::Vec2f>(Output_Height-1,Output_Width-1)[0] = apenture/2.0f;
    
    
    //rightmost and leftmost column
    for (int i = 0; i < Output_Height; i++) {
        flattable.at<cv::Vec2f>(i,0)[0] = -apenture/2.0f;
        flattable.at<cv::Vec2f>(i,0)[1] = 90 - stepY*i;
        
        flattable.at<cv::Vec2f>(i,Output_Width-1)[0] = apenture/2.0f;
        flattable.at<cv::Vec2f>(i,Output_Width-1)[1] = 90 - stepY*i;
        
    }
    flattable.at<cv::Vec2f>(Output_Height-1,0)[1] = -90;
    flattable.at<cv::Vec2f>(Output_Height-1,Output_Width-1)[1] = -90;
    
    
    //center row and column
    if (Output_Width%2 == 1) {
        int center = (Output_Width - 1)/2;
        for (int i = 0 ; i < Output_Height; i++) {
            flattable.at<cv::Vec2f>(i,center)[0] = 0;
        }
    }
    if (Output_Height%2 == 1) {
        int center = (Output_Height - 1)/2;
        for (int i = 0 ; i < Output_Width; i++) {
            flattable.at<cv::Vec2f>(center,i)[1] = 0;
        }
    }
}

//======================================================
//
//                  FishEyeRemaper
//
//======================================================
#pragma mark - FishEyeRemaper
cv::Mat FishEyeRemaper::remap(const cv::Mat inputImg){
    printf(" remaper:::::::\n");
    cv::Mat result = cv::Mat(flatindex.rows,flatindex.cols,inputImg.type());
    if (result.channels() == 1) {
        for (int i = 0; i < result.cols; i++) {
            for (int j = 0; j < result.rows; j++) {
                uchar aa = inputImg.at<uchar>(flatindex.at<cv::Vec2w>(j,i)[1],flatindex.at<cv::Vec2w>(j,i)[0]);
                uchar bb = inputImg.at<uchar>(flatindex.at<cv::Vec2w>(j,i)[1]+1,flatindex.at<cv::Vec2w>(j,i)[0]);
                uchar cc = inputImg.at<uchar>(flatindex.at<cv::Vec2w>(j,i)[1],flatindex.at<cv::Vec2w>(j,i)[0]+1);
                uchar dd = inputImg.at<uchar>(flatindex.at<cv::Vec2w>(j,i)[1]+1,flatindex.at<cv::Vec2w>(j,i)[0]+1);
                
                result.at<uchar>(j,i) = (uchar)((float)aa * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[0]+
                                                (float)bb * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[1]+
                                                (float)cc * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[2]+
                                                (float)dd * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[3]);
                
            }
        }
    }else if(result.channels() == 3){
        for (int i = 0; i < result.cols; i++) {
            for (int j = 0; j < result.rows; j++) {
                for (int k = 0; k < 3; k++) {
                    uchar aa = inputImg.at<cv::Vec3b>(flatindex.at<cv::Vec2w>(j,i)[1],flatindex.at<cv::Vec2w>(j,i)[0])[k];
                    uchar bb = inputImg.at<cv::Vec3b>(flatindex.at<cv::Vec2w>(j,i)[1]+1,flatindex.at<cv::Vec2w>(j,i)[0])[k];
                    uchar cc = inputImg.at<cv::Vec3b>(flatindex.at<cv::Vec2w>(j,i)[1],flatindex.at<cv::Vec2w>(j,i)[0]+1)[k];
                    uchar dd = inputImg.at<cv::Vec3b>(flatindex.at<cv::Vec2w>(j,i)[1]+1,flatindex.at<cv::Vec2w>(j,i)[0]+1)[k];
                    
                    
                    result.at<cv::Vec3b>(j,i)[k] = (uchar)((float)aa * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[0]+
                                                           (float)bb * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[1]+
                                                           (float)cc * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[2]+
                                                           (float)dd * factorTable.at<cv::Vec4f>(factorindex.at<cv::Vec2b>(j,i)[0],factorindex.at<cv::Vec2b>(j,i)[1])[3]);
                    
                    
                }
            }
        }
    }else{
        printf("channel is invalid~~~~");
    }
    
    return result;
}

FishEyeRemaper::~FishEyeRemaper(){

}



//======================================================
//
//                  FishEyeSphereRemaper
//
//======================================================

#pragma mark - FishEyeSphereRemaper

void  FishEyeSphereRemaper::compute(){
        printf("REC compute\n");
        initflatTable(apenture, outputsize);
        this->initflatindextable();
        initfactorTable();
       // logd();
       // logf();
}

void FishEyeSphereRemaper::initflatindextable(){
    if (Output_Width > 0 && Output_Height >0 && flattable.rows == Output_Height && flattable.cols == Output_Width) {
        flatindex = cv::Mat( Output_Height,Output_Width,CV_16UC2);
        factorindex = cv::Mat(Output_Height,Output_Width,CV_8UC2);
        float theta_x,theta_y,theta,phi,r,x_h,y_h;
        
        for (int i = 0; i < Output_Width; i++) {
            for (int j = 0; j < Output_Height; j++) {
                theta_x = flattable.at<cv::Vec2f>(j,i)[0];
                theta_y = flattable.at<cv::Vec2f>(j,i)[1];
                
                theta = acosf(cosf(theta_x*CV_PI/180.0)*cosf(theta_y*CV_PI/180.0));
                phi = acosf((cosf(theta_y*CV_PI/180.0)*sinf(theta_x*CV_PI/180.0))/sqrtf(cosf(theta_y*CV_PI/180.0)*cosf(theta_y*CV_PI/180.0)*sinf(theta_x*CV_PI/180.0)*sinf(theta_x*CV_PI/180.0)+sinf(theta_y*CV_PI/180.0)*sinf(theta_y*CV_PI/180.0)));
                r = this->getRadius(theta*180/CV_PI);
                x_h = r*cosf(phi);
                y_h = theta_y < 0 ? r*sinf(phi) : -r*sinf(phi);
                
                float x_i =x_h * 1000.0 / this->pixelsize;  //x_h in mm    pixelsize in um
                float y_i =y_h * 1000.0 / this->pixelsize;
                
                
                if ((center_y + y_i) < 0 || (center_y + y_i) > CMOS_size.height|| (center_x + x_i) > CMOS_size.width || (center_x + x_i) < 0) {
                    flatindex.at<cv::Vec2w>(j,i)[0] = 0;
                    flatindex.at<cv::Vec2w>(j,i)[1] = 0;
                    
                    float a = 0;
                    float b = 0;
                    
                    factorindex.at<cv::Vec2b>(j,i)[0] = (uchar)roundf(a * 31.0);
                    factorindex.at<cv::Vec2b>(j,i)[1] = (uchar)roundf(b * 31.0);
                }else{
                    flatindex.at<cv::Vec2w>(j,i)[0] = (ushort)floorf(center_x + x_i);
                    flatindex.at<cv::Vec2w>(j,i)[1] = (ushort)floorf(center_y + y_i);
                    
                    float a = center_x + x_i - floorf(center_x + x_i);
                    float b = center_y + y_i - floorf(center_y + y_i);
                    
                    factorindex.at<cv::Vec2b>(j,i)[0] = (uchar)roundf(a * 31.0);
                    factorindex.at<cv::Vec2b>(j,i)[1] = (uchar)roundf(b * 31.0);
                }

                
            }
        }
        
    }else{
        printf("flat table is not valid!");
    }
}

FishEyeSphereRemaper::~FishEyeSphereRemaper(){
    
}



//======================================================
//
//                  FishEyeRectRemaper
//
//======================================================
#pragma mark - FishEyeRectRemaper
void initflatRectTable(double apenture,unsigned int outputsize){
    Output_Width = 2*360*outputsize/apenture;
    Output_Height = outputsize;
    double stepX = 360.0/(float)(Output_Width-1);
    double stepY = (apenture/2.0)/(float)(Output_Height-1);
    
    //    ---------------------0
    //    |                   |
    //    |                   |
    //    |0               360|
    //    ---------------------apenture/2.0
    
    flattable = cv::Mat(Output_Height,Output_Width,CV_32FC2);
    for (int i = 1; i < Output_Width-1; i++) {
        for(int j = 1 ;j < Output_Height-1;j++){
            flattable.at<cv::Vec2f>(j,i)[0] =  stepX * i;
            flattable.at<cv::Vec2f>(j,i)[1] =  stepY * j;
        }
    }
    
    
    
    
    for (int i = 0; i < Output_Width;i++) {
        flattable.at<cv::Vec2f>(0,i)[0] = stepX*i;
        flattable.at<cv::Vec2f>(0,i)[1] = 0;
        
        flattable.at<cv::Vec2f>(Output_Height-1,i)[0] = stepX*i;
        flattable.at<cv::Vec2f>(Output_Height-1,i)[1] = apenture/2.0;
        
    }
    flattable.at<cv::Vec2f>(0,Output_Width-1)[0] = 360.0f;
    flattable.at<cv::Vec2f>(Output_Height-1,Output_Width-1)[0] = 360.0f;
    
    for (int i = 0; i < Output_Height; i++) {
        flattable.at<cv::Vec2f>(i,0)[0] = 0.0f;
        flattable.at<cv::Vec2f>(i,0)[1] = stepY*i;
        
        flattable.at<cv::Vec2f>(i,Output_Width-1)[0] = 360.0f;
        flattable.at<cv::Vec2f>(i,Output_Width-1)[1] = stepY*i;
        
    }
    flattable.at<cv::Vec2f>(Output_Height-1,0)[1] = apenture/2.0f;
    flattable.at<cv::Vec2f>(Output_Height-1,Output_Width-1)[1] = apenture/2.0f;
}
void  FishEyeRectRemaper::compute(){
    printf("compute\n");
    initflatRectTable(apenture, outputsize);
    this->initflatindextable();
    initfactorTable();
//    logd();
//    printf("=======================================================\n");
//    logf();
}

void FishEyeRectRemaper::initflatindextable(){
    if (Output_Width > 0 && Output_Height >0 && flattable.rows == Output_Height && flattable.cols == Output_Width) {
        flatindex = cv::Mat( Output_Height,Output_Width,CV_16UC2);
        factorindex = cv::Mat(Output_Height,Output_Width,CV_8UC2);
        float theta_x,theta_y,r,x_h,y_h;
        
        for (int i = 0; i < Output_Width; i++) {
            for (int j = 0; j < Output_Height; j++) {
                theta_x = flattable.at<cv::Vec2f>(j,i)[0];
                theta_y = flattable.at<cv::Vec2f>(j,i)[1];
                
                //theta = acosf(cosf(theta_x*CV_PI/180.0)*cosf(theta_y*CV_PI/180.0));
                //phi = acosf((cosf(theta_y*CV_PI/180.0)*sinf(theta_x*CV_PI/180.0))/sqrtf(cosf(theta_y*CV_PI/180.0)*cosf(theta_y*CV_PI/180.0)*sinf(theta_x*CV_PI/180.0)*sinf(theta_x*CV_PI/180.0)+sinf(theta_y*CV_PI/180.0)*sinf(theta_y*CV_PI/180.0)));
                r = this->getRadius(theta_y);
                x_h = r*sinf(theta_x*CV_PI/180.0);
                y_h = r*cosf(theta_x*CV_PI/180.0);
                
                float x_i =x_h * 1000.0 / this->pixelsize;  //x_h in mm    pixelsize in um
                float y_i =y_h * 1000.0 / this->pixelsize;
                
                if ((center_y + y_i) < 0 || (center_y + y_i) > CMOS_size.height|| (center_x + x_i) > CMOS_size.width || (center_x + x_i) < 0) {
                    flatindex.at<cv::Vec2w>(j,i)[0] = 0;
                    flatindex.at<cv::Vec2w>(j,i)[1] = 0;
                    
                    float a = 0;
                    float b = 0;
                    
                    factorindex.at<cv::Vec2b>(j,i)[0] = (uchar)roundf(a * 31.0);
                    factorindex.at<cv::Vec2b>(j,i)[1] = (uchar)roundf(b * 31.0);
                }else{
                    flatindex.at<cv::Vec2w>(j,i)[0] = (ushort)floorf(center_x + x_i);
                    flatindex.at<cv::Vec2w>(j,i)[1] = (ushort)floorf(center_y + y_i);
                    
                    float a = center_x + x_i - floorf(center_x + x_i);
                    float b = center_y + y_i - floorf(center_y + y_i);
                    
                    factorindex.at<cv::Vec2b>(j,i)[0] = (uchar)roundf(a * 31.0);
                    factorindex.at<cv::Vec2b>(j,i)[1] = (uchar)roundf(b * 31.0);
                }
                
            }
        }
        
    }else{
        printf("flat table is not valid!");
    }
}

FishEyeRectRemaper::~FishEyeRectRemaper(){
    
}

