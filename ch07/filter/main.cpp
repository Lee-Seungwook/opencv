#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void filter_embossing(); // 엠보싱 필터
void blurring_mean(); // 평균값 필터
void blurring_gaussian(); // 가우시안 필터
void unsharp_mask(); // 언샤프 마스크 필터
void noise_gaussian(); // 가우시안 잡음
void filter_bilateral(); // 양방향 필터
void filter_median(); // 미디언 필터링 

int main()
{
	// filter_embossing();
	// blurring_mean();
	// blurring_gaussian();
	// unsharp_mask();
	// noise_gaussian();
	// filter_bilateral();
	filter_median();
	return 0;
}

void filter_embossing()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	float data[] = { -1, -1, 0, -1, 0, 1, 0, 1, 1 }; // 필터 지정 (3 x 3)
	Mat emboss(3, 3, CV_32FC1, data); // 필터 적용

	Mat dst;
	filter2D(src, dst, -1, emboss, Point(-1, -1), 128);

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void blurring_mean()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	Mat dst;
	for (int ksize = 3; ksize <= 7; ksize += 2) { // 3, 5, 7 사이즈 필터를 반복문을 사용하여 각각 적용
		blur(src, dst, Size(ksize, ksize)); // 필터 적용
		
		String desc = format("Mean: %dx%d", ksize, ksize); // 필터 크기 문자열 설정
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255), 1, LINE_AA); // 영상에 이미지 출력

		imshow("dst", dst);
		waitKey(0);
	}

	destroyAllWindows();
}

void blurring_gaussian()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	Mat dst;
	for (int sigma = 1; sigma <= 5; sigma++) {
		GaussianBlur(src, dst, Size(), (double)sigma);

		String text = format("sigma = %d", sigma);
		putText(dst, text, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}

	destroyAllWindows();
}

void unsharp_mask()
{
	Mat src = imread("rose.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	for (int sigma = 1; sigma <= 5; sigma++) {
		Mat blurred;
		GaussianBlur(src, blurred, Size(), sigma);

		float alpha = 1.f; // alpha값을 통해 강조의 정도를 조절이 가능
		Mat dst = (1 + alpha) * src - alpha * blurred; // 언샤프 마스크 필터 수식 적용

		String desc = format("sigma: %d", sigma);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}

	destroyAllWindows();
}

void noise_gaussian()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	for (int stddev = 10; stddev <= 30; stddev += 10) {
		Mat noise(src.size(), CV_32SC1);
		randn(noise, 0, stddev); // 가우시안 잡음 발생, 표준 편차에 따라 잡음의 정도가 달라짐

		Mat dst;
		add(src, noise, dst, Mat(), CV_8U);

		String desc = format("stddev = %d", stddev);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255), 1, LINE_AA);
		imshow("dst", dst);
		waitKey();
	}

	destroyAllWindows();
}

void filter_bilateral()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat noise(src.size(), CV_32SC1);
	randn(noise, 0, 5); // 표준 편차가 5인 가우시안 잡음 추가
	add(src, noise, src, Mat(), CV_8U);

	Mat dst1;
	GaussianBlur(src, dst1, Size(), 5); // 표준 편차가 5인 가우시안 필터 적용

	Mat dst2;
	bilateralFilter(src, dst2, -1, 10, 5); // 색 공간의 표준 편차는 10, 좌표 공간의 표준 편차는 5를 사용하는 양방향 필터 적용

	imshow("src", src);
	imshow("dst1", dst1);
	imshow("dst2", dst2);

	waitKey();
	destroyAllWindows();
}

void filter_median()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	int num = (int)(src.total() * 0.1); // 영상의 10%
	for (int i = 0; i < num; i++) {
		int x = rand() % src.cols;
		int y = rand() % src.rows;
		src.at<uchar>(y, x) = (i % 2) * 255; // 임의의 좌표를 0또는 255로 바꿈
	}

	Mat dst1;
	GaussianBlur(src, dst1, Size(), 1); // 가우시안 필터링 수행

	Mat dst2;
	medianBlur(src, dst2, 3); // 미디언 필터 실행

	imshow("src", src);
	imshow("dst1", dst1);
	imshow("dst2", dst2);

	waitKey();
	destroyAllWindows();
}