#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

void Threshold(Mat& img)
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			int avg = (img.at<Vec3b>(i, j)[0] + img.at<Vec3b>(i, j)[1] + img.at<Vec3b>(i, j)[2]) / 3;
			if (avg > 128)
				img.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[2] = 255;
			else
				img.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[2] = 0;
		}
	}
	return;
}

void Grayscale(Mat& img)
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			double avg = 0.21*img.at<Vec3b>(i, j)[2]
				+ 0.72*img.at<Vec3b>(i, j)[1]
				+ 0.07*img.at<Vec3b>(i, j)[0];
			img.at<Vec3b>(i, j)[0] = img.at<Vec3b>(i, j)[1] = img.at<Vec3b>(i, j)[2] = avg;
		}
	}
	return;
}

//returns the numbers of the barcode
vector<int> GetInfo(Mat& img)
{
	vector<int> numbers;
	string code;
//	vector<string> binaryCodes;		// each string -> number

	Point start;
	Point end;

	for (int i = 0; i < img.rows - 1; ++i)
	{
		bool findStart = false;
		for (int j = 0; j < img.cols - 1; ++j)
		{
			if (img.at<Vec3b>(i, j)[0] == 0 &&
				img.at<Vec3b>(i + 1, j)[0] == 0 &&
				img.at<Vec3b>(i, j + 1)[0] == 0 &&
				img.at<Vec3b>(i + 1, j + 1)[0] == 0)
			{
				start = Point(i, j);
				findStart = true;
				break;
			}
		}
		if (findStart)
		{
			break;
		}
	}

	for (int i = 0; i < img.rows - 1; ++i)
	{
		bool findEnd = false;
		for (int j = img.cols - 1; j > 0; --j)
		{
			if (img.at<Vec3b>(i, j)[0] == 0 &&
				img.at<Vec3b>(i + 1, j)[0] == 0 &&
				img.at<Vec3b>(i, j - 1)[0] == 0 &&
				img.at<Vec3b>(i + 1, j - 1)[0] == 0)
			{
				end = Point(i, j);
				findEnd = true;
				break;
			}
		}
		if (findEnd)
		{
			break;
		}
	}

	cout << start << ' ' << endl;
	int length = end.y - start.y + 1;
	//	cout << length;

	int delta = length / 95;

	for (int i = 0; i < 95; ++i)
	{
		int count0 = 0;
		int count1 = 0;
		for (int j = 0; j < delta; ++j)
		{
			int ind = start.y + i * delta + j;
			if (img.at<Vec3b>(start.x, ind)[0] == 0)
				count1++;
			else
				count0++;
		}
		code.push_back(count0 > count1 ? '0' : '1');
	}
	cout << code << endl;
	//left side
	for (int i = 3; i < 45; i+=7)
	{
		int n;
		string s = code.substr(i, 7);
		if (s == "0001101")
			n = 0;
		if (s == "0011001")
			n = 1;
		if (s == "0010011")
			n = 2;
		if (s == "0111101")
			n = 3;
		if (s == "0100011")
			n = 4;
		if (s == "0110001")
			n = 5;
		if (s == "0101111")
			n = 6;
		if (s == "0111011")
			n = 7;
		if (s == "0110111")
			n = 8;
		if (s == "0001011")
			n = 9;
		numbers.push_back(n);
	}
	cout << numbers[0] << numbers[1] << numbers[2] << numbers[3] << numbers[4] << numbers[5] << endl;

	//right side
	//TODO

	return numbers;
}

//shows product information from the barcode numbers
void ShowInfo(vector<int> vec)
{

}

//recovers barcode from the barcode numbers
Mat RecoverBarcode(vector<int> vec)
{

	return Mat();
}

int main()
{
	Mat img = imread("example1.jpg");
	namedWindow("Window", WINDOW_AUTOSIZE & WINDOW_NORMAL);

	Threshold(img);
	
	vector<int> vec = GetInfo(img);

	imshow("Window", img);
	//	imwrite("", img);
	waitKey(0);
	return 0;
}