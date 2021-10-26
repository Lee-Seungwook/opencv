#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

void color_inverse(); // 트루 컬러 색상 반전
void color_grayscale(); // 그레이 스케일 영상으로 변환
void color_split(); // 트루 컬러 영상 색상 나누기
void color_req(); // 히스토그램 균등화

void color_inRange(); // 특정 색상 영역 분할
void on_hue_changed(int, void*); // 색상 영역 분할을 위한 객체를 생성

int lower_hue = 40, upper_hue = 80; // 두 개의 트랙바 위치를 저장할 정수형 변수
Mat src, src_hsv, mask; // Mat 객체

void color_backproj(); // 히스토그램 역투영을 이용한 피부색 영역 검출 




int main(int argc, char argv[])
{
	// color_inverse();
	// color_grayscale();
	// color_split();
	// color_req();
	// color_inRange();
	color_backproj();
}

void color_inverse()
{
	Mat src = imread("butterfly.jpg", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 반전 영상 출력할 객체 생성
	Mat dst(src.rows, src.cols, src.type());

	// 입력 영상 반전하여 출력 영상에 저장
	for (int j = 0; j < src.rows; j++) {
		for (int i = 0; i < src.cols; i++) {
			// dst.at<Vec3b>(j, i) = Vec3b(255, 255, 255) - src.at<Vec3b>(j, i); // 밑의 과정과 결과가 동일
			Vec3b& p1 = src.at<Vec3b>(j, i);
			Vec3b& p2 = dst.at<Vec3b>(j, i);

			p2[0] = 255 - p1[0]; // B
			p2[1] = 255 - p1[1]; // G
			p2[2] = 255 - p1[2]; // R
		}
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void color_grayscale()
{
	Mat src = imread("butterfly.jpg");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// RGB 영상을 그레이 스케일 영상으로 변환
	Mat dst;
	cvtColor(src, dst, COLOR_BGR2GRAY);

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void color_split()
{
	Mat src = imread("candies.png");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 색상 평면 나눠서 벡터에 저장
	vector<Mat> bgr_planes;
	split(src, bgr_planes);

	imshow("src", src);
	imshow("B_plane", bgr_planes[0]);
	imshow("G_plane", bgr_planes[1]);
	imshow("R_plane", bgr_planes[2]);

	waitKey();
	destroyAllWindows();
}

void color_req()
{
	Mat src = imread("pepper.bmp", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// BGR 색 공간의 입력 영상을 YCrCb 색 공간으로 변경
	Mat src_ycrcb;
	cvtColor(src, src_ycrcb, COLOR_BGR2YCrCb);

	// 영상의 채널을 분리하여 저장
	vector<Mat> ycrcb_planes;
	split(src_ycrcb, ycrcb_planes);

	// Y성분에 해당하는 영상에 대해서만 히스토그램 평활화 수행
	equalizeHist(ycrcb_planes[0], ycrcb_planes[0]);

	// 분할 된 영상을 다시 합침
	Mat dst_ycrcb;
	merge(ycrcb_planes, dst_ycrcb);

	// BGR영상으로 전환
	Mat dst;
	cvtColor(dst_ycrcb, dst, COLOR_YCrCb2BGR);

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void color_inRange()
{
	src = imread("candies.png", IMREAD_COLOR);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 영상을 HSV 공간으로 변환하여 저장(src_hsv)
	cvtColor(src, src_hsv, COLOR_BGR2HSV);

	imshow("src", src);

	// 색상의 하한 값과 상한 값을 조절할 수 있는 두 개의 트랙바를 생성
	// 색상의 최대값을 179로, 콜백 함수를 on_hue_changed 함수로 설정
	namedWindow("mask");
	createTrackbar("Lower Hue", "mask", &lower_hue, 179, on_hue_changed);
	createTrackbar("Upper Hue", "mask", &upper_hue, 179, on_hue_changed);
	on_hue_changed(0, 0); // 처음 실행될 때 영상이 정상적으로 출력되도록 트랙바 콜백 함수를 강제로 호출

	waitKey();
}

void on_hue_changed(int, void *)
{
	// 사용자가 지정한 색상의 하한 값과 상한 값을 이용하여 객체를 생성
	// 채도의 범위는 100 ~ 255, 명도의 범위는 0 ~ 255, 채도의 범위만 사용자가 설정하여 결과 영상을 출력
	Scalar lowerb(lower_hue, 100, 0);
	Scalar upperb(upper_hue, 255, 255);
	
	// HSV 색 성분의 범위가 lowerb부터 upperb 사이인 위치의 픽셀만 흰색으로 설정한 mask 영상을 생성 
	inRange(src_hsv, lowerb, upperb, mask);

	imshow("mask", mask);
}

void color_backproj()
{
	Mat ref, ref_ycrcb, mask;
	ref = imread("ref.png", IMREAD_COLOR);
	mask = imread("mask.bmp", IMREAD_GRAYSCALE);
	
	// 영상 변환
	cvtColor(ref, ref_ycrcb, COLOR_BGR2YCrCb);

	// 기준 영상에서 피부색 영역의 2차원 히스토그램을 계산하여 hist에 저장
	Mat hist;
	int channels[] = { 1, 2 };
	int cr_bins = 128; int cb_bins = 128;
	int histSize[] = { cr_bins, cb_bins };
	float cr_range[] = { 0, 256 };
	float cb_range[] = { 0, 256 };
	const float* ranges[] = { cr_range, cb_range };

	calcHist(&ref_ycrcb, 1, channels, mask, hist, 2, histSize, ranges);

	/*imshow("ref", ref);
	imshow("mask", mask);
	imshow("hist", hist);*/

	// Apply histogram backprojection to an input image

	Mat src, src_ycrcb;
	src = imread("kids.png", IMREAD_COLOR);
	cvtColor(src, src_ycrcb, COLOR_BGR2YCrCb);

	// 이전에 구한 hist의 히스토그램을 이용하여 입력 영상에서 히스토그램 역투영을 수행
	Mat backproj;
	calcBackProject(&src_ycrcb, 1, channels, hist, backproj, ranges, 1, true);

	imshow("src", src);
	imshow("backproj", backproj);
	waitKey();
}