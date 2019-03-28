#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

void Negative(Mat& img)
{
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			img.at<Vec3b>(i, j)[0] = 255 - img.at<Vec3b>(i, j)[0];
			img.at<Vec3b>(i, j)[1] = 255 - img.at<Vec3b>(i, j)[1];
			img.at<Vec3b>(i, j)[2] = 255 - img.at<Vec3b>(i, j)[2];
		}
	}
	return;
}

void AdaptiveBinarization(Mat& img)
{
	const int N = 50;
	int rows = img.rows / N;
	int cols = img.cols / N;
	for (int k = 0; k < N + 1; k++)
	{
		for (int l = 0; l < N + 1; l++)
		{
			long double cAvg = 0;
			int count = 0;
			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < cols; j++)
				{
					int i1 = k * rows + i;
					int j1 = l * cols + j;
					if (i1 >= img.rows || j1 >= img.cols)
						break;
					cAvg += (img.at<Vec3b>(i1, j1)[0] + img.at<Vec3b>(i1, j1)[1] + img.at<Vec3b>(i1, j1)[2]) / 3.0;
					++count;
				}
			}
			cAvg /= count;

			for (int i = 0; i < rows; i++)
			{
				for (int j = 0; j < cols; j++)
				{
					int i1 = k * rows + i;
					int j1 = l * cols + j;
					if (i1 >= img.rows || j1 >= img.cols)
						break;
					double avg = (img.at<Vec3b>(i1, j1)[0] + img.at<Vec3b>(i1, j1)[1] + img.at<Vec3b>(i1, j1)[2]) / 3.0;
					if (20 > avg ||  10 < cAvg - avg)
					{
						img.at<Vec3b>(i1, j1)[0] = img.at<Vec3b>(i1, j1)[1] = img.at<Vec3b>(i1, j1)[2] = 0;
					}
					else
					{
						img.at<Vec3b>(i1, j1)[0] = img.at<Vec3b>(i1, j1)[1] = img.at<Vec3b>(i1, j1)[2] = 255;
					}
				}
			}
		}
	}
}

void HoughTransform(Mat& img)
{
	Mat dst = img;
	Canny(img, dst, 100, 200, 3);
//	cvtColor(dst, img, COLOR_GRAY2BGR);

	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, 100, 0, 0);
	double avgTheta = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
//		cout << theta << endl;
		avgTheta += theta;
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));
//		line(img, pt1, pt2, Scalar(255, 0, 0), 5, LINE_AA);
	}
	//namedWindow("detected lines", WINDOW_AUTOSIZE & WINDOW_NORMAL);
	//imshow("detected lines", img);
//	AdaptiveBinarization(img);

	//needs optimization
	avgTheta /= lines.size();
	cout << avgTheta << endl;
	Mat r = getRotationMatrix2D(Point(img.cols / 2, img.rows / 2), -avgTheta, 1.0);
	Mat dst1;// (img.rows, img.cols, img.type(), Scalar(255, 255, 255));
//	Negative(img);
	warpAffine(img, dst1, r, img.size());
//	Negative(dst1);

//	namedWindow("rotated image", WINDOW_AUTOSIZE & WINDOW_NORMAL);
//	imshow("rotated image", dst1);
	AdaptiveBinarization(dst1);
//	imshow("binarized image", dst1);
	img = dst1;
}

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

//returns the code
string GetBinaryCode(Mat& img)
{
	string code;
	Point start;
	Point end;

	for (int i = 50; i < img.rows - 1; ++i)
	{
		bool findStart = false;
		for (int j = 1; j < img.cols - 1; ++j)
		{
			if (img.at<Vec3b>(i, j)[0] == 0 &&
				img.at<Vec3b>(i + 1, j)[0] == 0 &&
				img.at<Vec3b>(i, j + 1)[0] == 0 &&
				img.at<Vec3b>(i + 1, j + 1)[0] == 0 &&
				img.at<Vec3b>(i - 1, j)[0] == 0 &&
				img.at<Vec3b>(i - 1, j + 1)[0] == 0 &&
				img.at<Vec3b>(i - 1, j - 1)[0] == 255 &&
				img.at<Vec3b>(i, j - 1)[0] == 255 &&
				img.at<Vec3b>(i + 1, j - 1)[0] == 255)
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

	for (int i = 50; i < img.rows - 1; ++i)
	{
		bool findEnd = false;
		for (int j = img.cols - 1; j > 0; --j)
		{
			if (img.at<Vec3b>(i, j)[0] == 0 &&
				img.at<Vec3b>(i + 1, j)[0] == 0 &&
				img.at<Vec3b>(i, j - 1)[0] == 0 &&
				img.at<Vec3b>(i + 1, j - 1)[0] == 0 &&
				img.at<Vec3b>(i - 1, j)[0] == 0 && 
				img.at<Vec3b>(i - 1, j - 1)[0] == 0 && 
				img.at<Vec3b>(i - 1, j + 1)[0] == 255 && 
				img.at<Vec3b>(i, j + 1)[0] == 255 && 
				img.at<Vec3b>(i + 1, j + 1)[0] == 255)
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

	cout << start << ' ' << end << endl;
	int length = end.y - start.y + 1;
	cout << length << endl;

	double delta = length / 95.0;
	cout << delta << endl;

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

	return code;
}

//returns the numbers from binary code
vector<int> GetNumbers(string code)
{
	vector<int> numbers;
	//left side
	for (int i = 3; i < 45; i += 7)
	{
		int n = -1;
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
	cout << numbers[0] << numbers[1] << numbers[2] << numbers[3] << numbers[4] << numbers[5] << ends;

	//right side
	for (int i = 50; i < 92; i += 7)
	{
		int n = -1;
		string s = code.substr(i, 7);
		if (s == "1110010")
			n = 0;
		if (s == "1100110")
			n = 1;
		if (s == "1101100")
			n = 2;
		if (s == "1000010")
			n = 3;
		if (s == "1011100")
			n = 4;
		if (s == "1001110")
			n = 5;
		if (s == "1010000")
			n = 6;
		if (s == "1000100")
			n = 7;
		if (s == "1001000")
			n = 8;
		if (s == "1110100")
			n = 9;
		numbers.push_back(n);
	}
	std::cout << numbers[6] << numbers[7] << numbers[8] << numbers[9] << numbers[10] << numbers[11] << endl;
	return numbers;
}

vector<int> GetInfo(Mat& img)
{
	return GetNumbers(GetBinaryCode(img));
}

//shows product information from the barcode numbers
void ShowInfo(vector<int> vec)
{

}

//recovers barcode from the barcode numbers
//or from binary code?
Mat RecoverBarcode(string code)
{

	return Mat();
}

int main()
{
	Mat img = imread("example4.jpg");
//	namedWindow("Window", WINDOW_AUTOSIZE & WINDOW_NORMAL);

//	Threshold(img);	
//	cvtColor(dst, img, COLOR_GRAY2BGR);

	HoughTransform(img);
	AdaptiveBinarization(img);
	vector<int> vec = GetInfo(img);

	imshow("Window", img);
	//	imwrite("", img);
	waitKey(0);
	return 0;
}