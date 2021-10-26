#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void threshold(int argc, char* argv[]);// 이진화 작업
void on_threshold(int pos, void* userdata); // 임계값 결과 영상 출력

void adaptive_threshold(); // 적응형 이진화 작업
void on_trackbar(int pos, void* userdata); // 블록의 크기를 이용하여 적응형 이진화 작업 결과 영상 출력

void erode_dilate(); // 이진 영상의 침식과 팽창 
void open_close(); // 이진 영상의 열기와 닫기

int main(int argc, char* argv[])
{
	// threshold(argc, argv); // 명령행 인자 매개변수로 전달
	// adaptive_threshold();
	erode_dilate();
	// open_close();
}

void threshold(int argc, char* argv[])
{
	Mat src;

	// 명령행 인자 개수가 2보다 작으면 지정한 파일을 입력 영상으로 사용
	if (argc < 2)
		src = imread("neutrophils.png", IMREAD_GRAYSCALE);
	else // 프로그램 실행 파일 이름 뒤에 사용할 영상 파일 이름을 명시하면 해당 파일을 입력 영상으로 사용
		src = imread(argv[1], IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	// 트랙바를 생성
	namedWindow("dst");
	createTrackbar("Threshold", "dst", 0, 255, on_threshold, (void*)&src);
	setTrackbarPos("Threshold", "dst", 128); // 트랙바 위치가 128이 되도록 설정하여 on_threshold함수가 강제로 실행되도록 함

	waitKey();
}

void on_threshold(int pos, void* userdata)
{
	Mat src = *(Mat*)userdata;

	// 사용자가 지정한 트랙바 위치값을 이용하여 이진화를 수행하고 영상을 저장
	Mat dst;
	threshold(src, dst, pos, 255, THRESH_BINARY /*| THRESH_OTSU*/); // THRESH_OTSU등의 자동으로 임계값을 지정해주는 매크로를 사용하면 결과 영상만 출력된다.

	imshow("dst", dst);
	
}

void adaptive_threshold()
{
	Mat src = imread("sudoku.jpg", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	namedWindow("dst");
	createTrackbar("Block Size", "dst", 0, 200, on_trackbar, (void*)&src);
	setTrackbarPos("Block Size", "dst", 11);

	waitKey();
}

void on_trackbar(int pos, void* userdata)
{
	Mat src = *(Mat*)userdata;

	int bsize = pos;
	if (bsize % 2 == 0) bsize--;
	if (bsize < 3) bsize = 3;

	Mat dst;
	adaptiveThreshold(src, dst, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, bsize, 5);

	imshow("dst", dst);
}

void erode_dilate()
{
	Mat src = imread("milkdrop.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 오츠 알고리즘을 이용하여 자동 이진화를 수행하고 그 결과를 저장
	Mat bin;
	threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// 모폴로지 침식, 팽창 작업 수행
	Mat dst1, dst2, sub;
	erode(bin, dst1, Mat());
	dilate(bin, dst2, Mat());

	subtract(dst2, dst1, sub); // 뺄셈 연산을 이용하여 경계선 검출

	imshow("src", src);
	imshow("bin", bin);
	imshow("erode", dst1);
	imshow("dilate", dst2);
	imshow("sub", sub);

	waitKey();
	destroyAllWindows();
}

void open_close()
{
	Mat src = imread("milkdrop.bmp", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 오츠 알고리즘을 이용하여 자동 이진화를 수행하고 그 결과를 저장
	Mat bin;
	threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// 모폴로지 열기, 닫기 연산 수행 후 결과 저장
	Mat dst1, dst2;
	morphologyEx(bin, dst1, MORPH_OPEN, Mat());
	morphologyEx(bin, dst2, MORPH_CLOSE, Mat());

	imshow("src", src);
	imshow("bin", bin);
	imshow("erode", dst1);
	imshow("dilate", dst2);

	waitKey();
	destroyAllWindows();
}