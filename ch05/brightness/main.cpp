#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void brightness1();
void brightness2();
void on_brightness(int pos, void* userdata);
void brightness4();

int main() 
{
	// brightness1();
	//brightness2();
	brightness4();
}

void brightness1()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst = src + 100; // 밝기 조절 (100 증가)

	imshow("src", src);
	imshow("dst", dst);
	waitKey();

	destroyAllWindows();
}

void brightness2()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}
	
	Mat dst(src.rows, src.cols, src.type()); // 비어 있는 객체를 만들지 않기 위해 값을 지정

	for (int j = 0; j < src.rows; j++)
	for (int i = 0; i < src.cols; i++) {
		/*int v = src.at<uchar>(j, i) + 100;
		dst.at<uchar>(j, i) = v > 255 ? 255 : v < 0? 0 : v;*/
		dst.at<uchar>(j, i) = saturate_cast<uchar>(src.at<uchar>(j, i) + 100); // 위의 주석 코드와 결과는 동일
	}

	imshow("src", src);
	imshow("dst", dst);
	waitKey();

	destroyAllWindows();
}

void brightness4()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	namedWindow("dst");
	createTrackbar("Brightness", "dst", 0, 100, on_brightness, (void*)&src);
	on_brightness(0, (void*)&src);

	waitKey();
	destroyAllWindows();
}

void on_brightness(int pos, void* userdata)
{
	Mat src = *(Mat*)userdata;
	Mat dst = src + pos;

	imshow("dst", dst);
}