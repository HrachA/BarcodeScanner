// Hrach Ayunc, Artyom Hakobyan, Toma Hovhannisyan
// Barcode decoder and retriever

#include<opencv2/opencv.hpp>
#include<iostream>
#include<vector>

using namespace std;
using namespace cv;

void MinFilter(Mat& img)
{
	Mat exp = img.clone();
	for (int i = 1; i < img.rows - 1; i++)
	{
		for (int j = 1; j < img.cols - 1; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				int a[9] = { exp.at<Vec3b>(i - 1, j - 1)[k], exp.at<Vec3b>(i - 1, j)[k], exp.at<Vec3b>(i - 1, j + 1)[k],
					exp.at<Vec3b>(i, j - 1)[k], exp.at<Vec3b>(i, j)[k], exp.at<Vec3b>(i, j + 1)[k],
					exp.at<Vec3b>(i + 1, j - 1)[k], exp.at<Vec3b>(i + 1, j)[k], exp.at<Vec3b>(i + 1, j + 1)[k] };
				sort(a, a + 9);
				img.at<Vec3b>(i, j)[k] = a[0];
			}
		}
	}
}

void MedianFilter(Mat& img)
{
	Mat exp = img.clone();
	for (int i = 1; i < img.rows - 1; i++)
	{
		for (int j = 1; j < img.cols - 1; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				int a[9] = { exp.at<Vec3b>(i - 1, j - 1)[k], exp.at<Vec3b>(i - 1, j)[k], exp.at<Vec3b>(i - 1, j + 1)[k],
					exp.at<Vec3b>(i, j - 1)[k], exp.at<Vec3b>(i, j)[k], exp.at<Vec3b>(i, j + 1)[k],
					exp.at<Vec3b>(i + 1, j - 1)[k], exp.at<Vec3b>(i + 1, j)[k], exp.at<Vec3b>(i + 1, j + 1)[k] };
				sort(a, a + 9);
				img.at<Vec3b>(i, j)[k] = a[4];
			}
		}
	}
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
					if (50 > avg || 10 < cAvg - avg)
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
	Mat dst;
	Mat lineSave = img.clone();

	Canny(img, dst, 100, 200, 3);
//	imshow("canny", dst);
//	imwrite("canny.jpg", dst);

	int numberOfPoints = 100;
	numberOfPoints += (img.rows*img.cols - 40000) * 200 / (3000000 - 40000);
//	cout << "Number of points: " << numberOfPoints << endl;
//	cout << "Pixture size: " << img.size() << endl;
	vector<Vec2f> lines;
	HoughLines(dst, lines, 1, CV_PI / 180, numberOfPoints, 0, 0);
	double avgTheta = 0;
	for (size_t i = 0; i < lines.size(); i++)
	{
		float rho = lines[i][0], theta = lines[i][1];
//		cout << theta << ends;

		if (theta > CV_PI / 2)
			theta = -CV_PI + theta;
//		cout << theta << endl;

		avgTheta += theta;
		Point pt1, pt2;
		double a = cos(theta), b = sin(theta);
		double x0 = a * rho, y0 = b * rho;
		pt1.x = cvRound(x0 + 1000 * (-b));
		pt1.y = cvRound(y0 + 1000 * (a));
		pt2.x = cvRound(x0 - 1000 * (-b));
		pt2.y = cvRound(y0 - 1000 * (a));

		line(lineSave, pt1, pt2, Scalar(255, 0, 0), 5, LINE_AA);
	}
//	namedWindow("detected lines", WINDOW_AUTOSIZE & WINDOW_NORMAL);
//	imshow("detected lines", lineSave);
//	imwrite("lines.jpg", lineSave);


	avgTheta /= lines.size();
//	cout << "Average theta before: " << avgTheta << std::endl;
	if (avgTheta > CV_PI / 2)
		avgTheta = -CV_PI + avgTheta;

	avgTheta = avgTheta * 180 / CV_PI;
//	cout << "Average theta after: " << avgTheta << endl;
	if (avgTheta >= -10 && avgTheta <= 10)
		avgTheta = 0;

	Mat r = getRotationMatrix2D(Point(img.cols / 2, img.rows / 2), avgTheta, 1.0);
	Mat rotatedImg;
	warpAffine(img, rotatedImg, r, img.size());

//	namedWindow("rotated image", WINDOW_AUTOSIZE & WINDOW_NORMAL);
//	imshow("rotated image", rotatedImg);
//	imwrite("rotated.jpg", rotatedImg);
	img = rotatedImg;
}

//returns the code
string GetBinaryCode(Mat& img)
{
	string code;
	Point start;
	Point end;

	for (int i = img.rows / 2 + 10; i < img.rows - 1; ++i)
	{
		bool findStart = false;
		for (int j = 1; j < img.cols - 1; ++j)
		{
			if (img.at<Vec3b>(i, j)[0] == 0 &&
				img.at<Vec3b>(i, j + 1)[0] == 0 &&
				img.at<Vec3b>(i + 1, j + 1)[0] == 0 &&
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

	for (int i = img.rows / 2 + 10; i < img.rows - 1; ++i)
	{
		bool findEnd = false;
		for (int j = img.cols - 2; j > 0; --j)
		{
			if (img.at<Vec3b>(i, j)[0] == 0 &&
				img.at<Vec3b>(i, j - 1)[0] == 0 &&
				img.at<Vec3b>(i + 1, j - 1)[0] == 0 &&
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

	Point p1(start.y, start.x);
	Point p2(end.y, end.x);

	//	line(img, p1, p2, Scalar(255, 0, 0), 5, LINE_AA);

//	cout << "Points: " << start << ' ' << end << endl;
	int length = end.y - start.y + 1;
//	cout << "Length: " << length << endl;

	double delta = length / 95.0;
//	cout << "Delta: " << delta << endl;

	for (int i = 0; i < 95; ++i)
	{
		int count0 = 0;
		int count1 = 0;
		for (int j = 0; j < delta; ++j)
		{
			int ind = start.y + i * delta + j;
			if (img.at<Vec3b>(start.x, ind)[0] < 10)
				count1++;
			else
				count0++;

			if (img.at<Vec3b>(start.x - 5, ind)[0] < 10)
				count1++;
			else
				count0++;

			if (img.at<Vec3b>(start.x + 5, ind)[0] < 10)
				count1++;
			else
				count0++;
		}
		code.push_back(count0 > count1 ? '0' : '1');
	}

	line(img, p1, p2, Scalar(0, 255, 0), 1, LINE_AA);
//	imshow("Line", img);
//	cout << code << endl;
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
	return numbers;
}

vector<int> GetInfo(Mat& img)
{
	return GetNumbers(GetBinaryCode(img));
}

//recovers barcode from binary code
Mat RecoverBarcode(string code)
{
	Mat BarCode(400, 95 * 5 + 40, CV_8UC3, Scalar(255, 255, 255));

	for (int i = 0; i < 95; i++)
	{
		if (code[i] == '1')
		{
			line(BarCode, Point(i * 5 + 20, 20), Point(i * 5 + 20, 360), Scalar(0, 0, 0), 1);
			line(BarCode, Point(i * 5 + 21, 20), Point(i * 5 + 21, 360), Scalar(0, 0, 0), 1);
			line(BarCode, Point(i * 5 + 22, 20), Point(i * 5 + 22, 360), Scalar(0, 0, 0), 1);
			line(BarCode, Point(i * 5 + 23, 20), Point(i * 5 + 23, 360), Scalar(0, 0, 0), 1);
			line(BarCode, Point(i * 5 + 24, 20), Point(i * 5 + 24, 360), Scalar(0, 0, 0), 1);
		}
	}
	vector<int> vec = GetNumbers(code);
	string s;
	for (auto const& value : vec)
	{
		s.push_back(static_cast<char>(value + '0'));
	}
	cout << "Numbers: " << s << endl;
	putText(BarCode, s, Point(20, 380),
		FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0, 0, 0), 1.5, LINE_AA);
	return BarCode;
}

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
	waitKey(0);
	return 0;
}