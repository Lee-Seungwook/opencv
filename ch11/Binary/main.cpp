#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void threshold(int argc, char* argv[]);// ����ȭ �۾�
void on_threshold(int pos, void* userdata); // �Ӱ谪 ��� ���� ���

void adaptive_threshold(); // ������ ����ȭ �۾�
void on_trackbar(int pos, void* userdata); // ����� ũ�⸦ �̿��Ͽ� ������ ����ȭ �۾� ��� ���� ���

void erode_dilate(); // ���� ������ ħ�İ� ��â 
void open_close(); // ���� ������ ����� �ݱ�

int main(int argc, char* argv[])
{
	// threshold(argc, argv); // ����� ���� �Ű������� ����
	// adaptive_threshold();
	erode_dilate();
	// open_close();
}

void threshold(int argc, char* argv[])
{
	Mat src;

	// ����� ���� ������ 2���� ������ ������ ������ �Է� �������� ���
	if (argc < 2)
		src = imread("neutrophils.png", IMREAD_GRAYSCALE);
	else // ���α׷� ���� ���� �̸� �ڿ� ����� ���� ���� �̸��� ����ϸ� �ش� ������ �Է� �������� ���
		src = imread(argv[1], IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	imshow("src", src);

	// Ʈ���ٸ� ����
	namedWindow("dst");
	createTrackbar("Threshold", "dst", 0, 255, on_threshold, (void*)&src);
	setTrackbarPos("Threshold", "dst", 128); // Ʈ���� ��ġ�� 128�� �ǵ��� �����Ͽ� on_threshold�Լ��� ������ ����ǵ��� ��

	waitKey();
}

void on_threshold(int pos, void* userdata)
{
	Mat src = *(Mat*)userdata;

	// ����ڰ� ������ Ʈ���� ��ġ���� �̿��Ͽ� ����ȭ�� �����ϰ� ������ ����
	Mat dst;
	threshold(src, dst, pos, 255, THRESH_BINARY /*| THRESH_OTSU*/); // THRESH_OTSU���� �ڵ����� �Ӱ谪�� �������ִ� ��ũ�θ� ����ϸ� ��� ���� ��µȴ�.

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

	// ���� �˰����� �̿��Ͽ� �ڵ� ����ȭ�� �����ϰ� �� ����� ����
	Mat bin;
	threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// �������� ħ��, ��â �۾� ����
	Mat dst1, dst2, sub;
	erode(bin, dst1, Mat());
	dilate(bin, dst2, Mat());

	subtract(dst2, dst1, sub); // ���� ������ �̿��Ͽ� ��輱 ����

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

	// ���� �˰����� �̿��Ͽ� �ڵ� ����ȭ�� �����ϰ� �� ����� ����
	Mat bin;
	threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// �������� ����, �ݱ� ���� ���� �� ��� ����
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