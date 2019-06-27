#ifndef __BARCODE_H__
#define __BARCODE_H__

#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;


void MinFilter(Mat& img);

void MedianFilter(Mat& img);

void AdaptiveBinarization(Mat& img);

//hough transform and rotating
void HoughTransform(Mat& img);

//returns the binary code
string GetBinaryCode(Mat& img);

//returns the numbers from binary code
vector<int> GetNumbers(string code);

//returns the numbers from source image
vector<int> GetInfo(Mat& img);

//recovers barcode from binary code
Mat RecoverBarcode(string code);

#endif //__BARCODE_H__
