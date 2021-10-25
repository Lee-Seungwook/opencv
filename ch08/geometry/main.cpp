#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void affine_transform(); // 어파인 변환
void affine_translation(); // 이동 변환
void affine_shear(); // 전단 변환
void affine_scale(); // 크기 변환
void affine_rotation(); // 회전 변환
void affine_flip(); // 대칭 변환

int main()
{
	// affine_transform(); 
	// affine_translation();
	// affine_shear();
	// affine_scale();
	// affine_rotation(); 
	affine_flip();
}

void affine_transform()
{
	Mat src = imread("tekapo.bmp");

	if (src.empty()) {
		cerr << "Image load failed" << endl;
		return;
	}

	// 입 * 출력 영상의 어파인 변환을 수행 할 세 점의 좌표를 저장
	Point2f srcPts[3], dstPts[3];
	srcPts[0] = Point2f(0, 0);
	srcPts[1] = Point2f(src.cols - 1, 0);
	srcPts[2] = Point2f(src.cols - 1, src.rows - 1);
	dstPts[0] = Point2f(50, 50);
	dstPts[1] = Point2f(src.cols - 100, 100);
	dstPts[2] = Point2f(src.cols - 50, src.rows - 50);

	// 2 * 3 어파인 변환 행렬을 M에 저장
	Mat M = getAffineTransform(srcPts, dstPts);

	// 어파인 변환 행렬 M을 이용하여, 입력 영상을 어파인 변환하여 출력에 저장, 
	// Size()를 사용하여 출력 영상이 입력 영상의 크기와 같도록 함
	Mat dst;
	warpAffine(src, dst, M, Size());

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void affine_translation()
{
	Mat src = imread("tekapo.bmp");

	if (src.empty()) {
		cerr << "Image load failed" << endl;
		return;
	}

	// 가로로 150픽셀, 세로로 100픽셀 이동하는 어파인 변환 행렬 M을 생성
	Mat M = Mat_<double>({ 2, 3 }, { 1, 0, 150, 0, 1, 100 });

	// 입력 영상을 이동 변환하여 출력 영상을 생성
	Mat dst;
	warpAffine(src, dst, M, Size() /*Size(1000, 1000)*/);

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void affine_shear()
{
	Mat src = imread("tekapo.bmp");

	if(src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	double mx = 0.3;
	Mat M = Mat_<double>({ 2, 3 }, { 1, mx, 0, 0, 1, 0 });

	Mat dst;
	warpAffine(src, dst, M, Size(cvRound(src.cols + src.rows * mx), src.rows));

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void affine_scale()
{
	Mat src = imread("rose.bmp");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat dst1, dst2, dst3, dst4;
	resize(src, dst1, Size(), 4, 4, INTER_NEAREST); // 최근방 보간법
	resize(src, dst2, Size(1920, 1280)); // default인 양선형 보간법
	resize(src, dst3, Size(1920, 1280), 0, 0, INTER_CUBIC); // 3차 회선 보간법
	resize(src, dst4, Size(1920, 1280), 0, 0, INTER_LANCZOS4); // 란초스 보간법

	// 좌표 400, 500 부터 400 * 500 사이즈의 영상을 출력
	imshow("src", src);
	imshow("dst1", dst1(Rect(400, 500, 400, 500)));
	imshow("dst2", dst2(Rect(400, 500, 400, 500)));
	imshow("dst3", dst3(Rect(400, 500, 400, 500)));
	imshow("dst4", dst4(Rect(400, 500, 400, 500)));

	waitKey();
	destroyAllWindows();
}

void affine_rotation()
{
	Mat src = imread("tekapo.bmp");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Point2f cp(src.cols / 2.f, src.rows / 2.f); // 영상의 중심 좌표를 가리키는 변수
	Mat M = getRotationMatrix2D(cp, 20, 1); // 반시계 방향으로 20도 회전하는 변환 행렬 생성

	Mat dst;
	warpAffine(src, dst, M, Size()); // 영상을 회전시키는 어파인 변환 수행

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void affine_flip()
{
	Mat src = imread("eastsea.bmp");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	Mat dst;
	int flipCode[] = { 1, 0, -1 };
	for (int i = 0; i < 3; i++) {
		flip(src, dst, flipCode[i]);

		String desc = format("flipCode : %d", flipCode[i]);
		putText(dst, desc, Point(10, 30), FONT_HERSHEY_SIMPLEX, 1.0, Scalar(255, 0, 0), 1, LINE_AA);

		imshow("dst", dst);
		waitKey();
	}

	destroyAllWindows();
}