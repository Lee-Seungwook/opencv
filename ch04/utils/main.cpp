#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void mask_setTo();
void mask_copyTo();
void time_inverse();

int main()
{
	// mask_setTo();
	// mask_copyTo();
	time_inverse();
}

void mask_setTo()
{
	Mat src = imread("lenna.bmp", IMREAD_COLOR);
	Mat mask = imread("mask_smile.bmp", IMREAD_GRAYSCALE);

	if (src.empty() || mask.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	src.setTo(Scalar(0, 255, 255), mask); // 마스크 연산을 이용

	imshow("src", src);
	imshow("mask", mask);

	waitKey(0);
	destroyAllWindows();
}

void mask_copyTo()
{
	Mat src = imread("airplane.bmp", IMREAD_COLOR);
	Mat mask = imread("mask_plane.bmp", IMREAD_GRAYSCALE);
	Mat dst = imread("field.bmp", IMREAD_COLOR);

	if (src.empty() || mask.empty() || dst.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);
	imshow("mask", mask);

	src.copyTo(dst, mask); // 첫 번째 매개변수 영상에 두 번째 매개변수 영상 영역과 일치하는 this를 첫 번째 영상에 복사

	imshow("dst", dst);

	waitKey(0);
	destroyAllWindows();
}

void time_inverse()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst(src.rows, src.cols, src.type());

	imshow("src", src);
	TickMeter tm;
	tm.start();

	for (int j = 0; j < src.rows; j++)
		for (int i = 0; i < src.cols; i++)
			dst.at<uchar>(j, i) = 255 - src.at<uchar>(j, i);

	tm.stop();

	cout << "Image inverse took" << tm.getTimeMilli() << "ms." << endl;

	imshow("dst", dst);

	waitKey(0);
	destroyAllWindows();


}