#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void labeling_basic(); // 라벨링 작동 원리
void labeling_stats(); // 라벨링을 이용하여 객체의 바운딩 박스 그리기
void contours_basic(); // 외곽선 검출과 그리기
void contours_hier(); // 계층 구조를 사용하는 외곽선 검출과 그리기

void contours_labeling(int argc, char* argv[]); // 다각형 검출 및 인식
void setLabel(Mat& img, const vector<Point>& pts, const String& label); // 라벨링 진행

int main(int argc, char* argv[])
{
	// labeling_basic();
	// labeling_stats();
	// contours_basic();
	// contours_hier();
	contours_labeling(argc, argv);
}

void labeling_basic()
{
	// 그레이 스케일 영상 생성
	uchar data[] = {
		0, 0, 1, 1, 0, 0, 0, 0,
		1, 1, 1, 1, 0, 0, 1, 0,
		1, 1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 1, 0,
		0, 0, 0, 1, 1, 1, 1, 0,
		0, 0, 0, 1, 0, 0, 1, 0,
		0, 0, 1, 1, 1, 1, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 0,
	};

	// data 행렬의 모든 원소에 255를 곱한 결과를 저장
	Mat src = Mat(8, 8, CV_8UC1, data) * 255;

	// 레이블 맵을 생성하여 저장
	Mat labels;
	int cnt = connectedComponents(src, labels); // 생성된 레이블 맵 객체 수를 저장

	cout << "src:\n" << src << endl;
	cout << "labels:\n" << labels << endl;
	cout << "number of labels: " << cnt << endl;
}

void labeling_stats()
{
	Mat src = imread("keyboard.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 오츠 알고리즘을 이용하여 입력 영상의 이진화 작업 진행
	Mat bin;
	threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// 이진화 영상에 대해 레이블링을 수행하고 각 객체 영역의 통계 정보를 추출
	Mat labels, stats, centroids;
	int cnt = connectedComponentsWithStats(bin, labels, stats, centroids);

	// 입력 영상을 컬러로 변환
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// 흰색 객체 영역에 대해서만 for 반복문 수행
	for (int i = 0; i < cnt; i++) {
		int* p = stats.ptr<int>(i);

		// 객체의 픽셀 개수가 20 보다 작으면 잡음이라고 간주하여 무시
		if (p[4] < 20) continue;

		// 노란색 사각형을 그린다
		rectangle(dst, Rect(p[0], p[1], p[2], p[3]), Scalar(0, 255, 255), 2);
	}

	imshow("src", src);
	imshow("bin", bin);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void contours_basic()
{
	Mat src = imread("contours.bmp", IMREAD_GRAYSCALE); // 해당 영상은 이진화 작업이 적용된 영상이므로, 다른 영상을 사용할 경우 이진화 작업이 별도 추가 되어야 할듯 함

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 모든 외곽선 검출, 계층 정보는 추출하지 않는다.
	vector<vector<Point>> contours;
	findContours(src, contours, RETR_LIST, CHAIN_APPROX_NONE);

	// 컬러 영상으로 변환
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// contours에 저장된 각각의 외곽선을 랜덤한 색상으로 그려준다.
	for (int i = 0; i < contours.size(); i++) {
		Scalar c(rand() & 255, rand() & 255, rand() % 255);
		drawContours(dst, contours, i, c, 2);
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void contours_hier()
{
	Mat src = imread("contours.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 외곽선 검출 및 계층 정보를 받아옴 (RETR_CCOMP를 사용하여 2층의 계층구조 생성)
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	// 컬러 영상으로 변환
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// 계층 구조의 순서대로 외곽선 내부를 색깔로 채움 (선의 두께가 -1이기 때문에)
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
		Scalar c(rand() & 255, rand() & 255, rand() & 255);
		drawContours(dst, contours, idx, c, -1, LINE_8, hierarchy);
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void contours_labeling(int argc, char* argv[])
{
	Mat img = imread("polygon.bmp", IMREAD_COLOR);

	if (img.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	Mat gray;
	cvtColor(img, gray, COLOR_BGR2GRAY);

	// 오츠 알고리즘을 이용한 이진화 작업 수행
	Mat bin;
	threshold(gray, bin, 200, 255, THRESH_BINARY_INV | THRESH_OTSU);

	// 외곽선 검출 수행
	vector<vector<Point>> contours;
	findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	// 각 객체의 외곽선 좌표를 pts 변수로 참조
	for (vector<Point>& pts : contours) {
		// 면적이 400보다 작으면 잡음으로 간주
		if (contourArea(pts) < 400)
			continue;

		// 외곽선을 근사화하여 저장
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true)*0.02, true);

		// 외곽선 점의 개수를 저장
		int vtc = (int)approx.size();

		// 꼭지점의 개수에 따라
		if (vtc == 3) {
			setLabel(img, pts, "TRI");
		}
		else if (vtc == 4) {
			setLabel(img, pts, "RECT");
		}
		// 면적 대 길이 비율을 조사
		else if (vtc > 4) {
			double len = arcLength(pts, true);
			double area = contourArea(pts);
			double ratio = 4. * CV_PI * area / (len * len);

			if (ratio > 0.8) {
				setLabel(img, pts, "CIR");
			}
		}
	}

	imshow("img", img);

	waitKey();
	destroyAllWindows();
}

void setLabel(Mat& img, const vector<Point>& pts, const String& label)
{
	// 외곽선 주변에 바운딩 박스를 그리고 label 문자열을 출력
	Rect rc = boundingRect(pts);
	rectangle(img, rc, Scalar(0, 0, 255), 1); // 바운딩 박스를 주황색으로 표시
	putText(img, label, rc.tl(), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255)); // 박스 좌측 상단에 문자열을 출력
}