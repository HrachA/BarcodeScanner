#include "Barcode.h"

int main()
{
	Mat img = imread("example9.png");

	AdaptiveBinarization(img);
	HoughTransform(img);
	MedianFilter(img);
	AdaptiveBinarization(img);
	string code = GetBinaryCode(img);
	Mat newImg = RecoverBarcode(code);

	imshow("Window", newImg);
	imwrite("recovered.jpg", newImg);
	waitKey(0);
	return 0;
}