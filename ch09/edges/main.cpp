#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void sobel_edge(); // 소벨 엣지
void canny_edge(); // 캐니 엣지
void hough_lines(); // 허프 변환 직선 검출
void hough_line_segments(); // 확률적 허프 변환 선분 검출
void hough_circles(); // 허프 원 검출

int main()
{
	// sobel_edge();
	// canny_edge();
	// hough_lines();
	// hough_line_segments();
	hough_circles();
}

void sobel_edge()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// x, y축 1차 편미분을 구하여 갑을 저장
	Mat dx, dy;
	Sobel(src, dx, CV_32FC1, 1, 0);
	Sobel(src, dy, CV_32FC1, 0, 1);

	Mat fmag, mag;
	magnitude(dx, dy, fmag); // 그래디언트 크기를 계산하여 fmag에 저장
	fmag.convertTo(mag, CV_8UC1); // 실수형 행렬 fmag을 그레이스케일 형식으로 변환하여 mag에 저장

	Mat edge = mag > 150; // 임계값 설정하여 행렬 원소 값을 임계값과 비교하여 저장

	imshow("src", src);
	imshow("mag", mag);
	imshow("edge", edge);

	waitKey();
	destroyAllWindows();
}

void canny_edge()
{
	Mat src = imread("lenna.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 최소 임계값 50, 최대 임계값은 각각 100, 150으로 설정
	Mat dst1, dst2;
	Canny(src, dst1, 50, 100); // 가우시안 필터링 -> 그래디언트 계산 -> 비최대 억제 -> 이중 임계값을 이용한 히스테리시스 에지 트래킹
	Canny(src, dst2, 50, 150);

	imshow("src", src);
	imshow("dst1", dst1);
	imshow("dst2", dst2);

	waitKey();
	destroyAllWindows();
}

void hough_lines()
{
	Mat src = imread("building.jpg", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 캐니 엣지 검출 결과를 저장
	Mat edge;
	Canny(src, edge, 50, 150);

	// 직선의 방정식 파라미터 p와 세타 정보를 lines에 저장
	vector<Vec2f> lines; // 자료형은 주로 Vec2f 또는 Vec3f를 사용
	HoughLines(edge, lines, 1, CV_PI / 180, 250); // 입력 영상, 직선 정보를 저장할 출력 벡터, p값의 해상도, 세타 값의 해상도, 직선으로 판단할 임계값

	// 그레이스케일 edge 영상을 BGR 3채널 컬러 영상으로 변환하여 저ㅏㅇ
	Mat dst;
	cvtColor(edge, dst, COLOR_GRAY2BGR);

	// 영상에 붉은 직선을 그리기 위한 반복문
	for (size_t i = 0; i < lines.size(); i++) {
		float r = lines[i][0], t = lines[i][1]; // p를 변수 r에, 세타를 변수 t에 저장
		double cos_t = cos(t), sin_t = sin(t);
		double x0 = r * cos_t, y0 = r * sin_t;
		double alpha = 100; // 값을 충분히 크게 설정해야 자연스러운 직선을 그릴 수 있다.

		// 직선 상의 두 점 좌표를 구한다.
		Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
		Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
		line(dst, pt1, pt2, Scalar(0, 0, 255), 2, LINE_AA); // 직선 그리기
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey(0);
	destroyAllWindows();
}

void hough_line_segments()
{
	Mat src = imread("building.jpg", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 캐니 엣지 검출 결과 저장
	Mat edge;
	Canny(src, edge, 50, 150);

	// 모든 직선 성분의 시작점과 끝점 좌표를 구함
	vector<Vec4i> lines;
	HoughLinesP(edge, lines, 1, CV_PI / 180, 160, 50, 5);

	// 컬러 영상으로 전환하여 저장
	Mat dst;
	cvtColor(edge, dst, COLOR_GRAY2BGR);

	// 구해진 모든 직선 성분(좌표)을 출력 영상 위에 빨간색 직선으로 그린다.
	for (Vec4i l : lines) {
		line(dst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 2, LINE_AA);
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void hough_circles()
{
	Mat src = imread("coins.png", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed" << endl;
		return;
	}

	// 잡음을 제거
	Mat blurred;
	blur(src, blurred, Size(3, 3));

	// 원을 검출, 
	// (입력 영상, 출력 영상, 방식은 default, 축적 배열은 입력 영상과 동일, 최소 임계값, 최대 임계값, 축적 배열 원소 값의 임계값(임계값 보다 크면 원의 중심으로))
	vector<Vec3f> circles;
	HoughCircles(blurred, circles, HOUGH_GRADIENT, 1, 50, 150, 30);

	// 컬러 영상으로 변환
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// 원 그리기 반복문
	for (Vec3f c : circles) {
		Point center(cvRound(c[0]), cvRound(c[1]));
		int radius = cvRound(c[2]);
		circle(dst, center, radius, Scalar(0, 0, 255), 2, LINE_AA);
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}