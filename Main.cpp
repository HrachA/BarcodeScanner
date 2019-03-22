#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;


//returns the numbers of the barcode
vector<int> GetInfo(Mat& img)
{

	return vector<int>();
}

//shows product information from the barcode numbers
void showInfo(vector<int> vec)
{

}

//recovers barcode from the barcode numbers
Mat recoverBarcode(vector<int> vec)
{

	return Mat();
}

int main()
{
	Mat img = imread("example2.png");
	namedWindow("Window", WINDOW_AUTOSIZE & WINDOW_NORMAL);

	

	imshow("Window", img);
	//	imwrite("", img);
	waitKey(0);
	return 0;
}