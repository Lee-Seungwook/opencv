#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void sobel_edge(); // �Һ� ����
void canny_edge(); // ĳ�� ����
void hough_lines(); // ���� ��ȯ ���� ����
void hough_line_segments(); // Ȯ���� ���� ��ȯ ���� ����
void hough_circles(); // ���� �� ����

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

	// x, y�� 1�� ���̺��� ���Ͽ� ���� ����
	Mat dx, dy;
	Sobel(src, dx, CV_32FC1, 1, 0);
	Sobel(src, dy, CV_32FC1, 0, 1);

	Mat fmag, mag;
	magnitude(dx, dy, fmag); // �׷����Ʈ ũ�⸦ ����Ͽ� fmag�� ����
	fmag.convertTo(mag, CV_8UC1); // �Ǽ��� ��� fmag�� �׷��̽����� �������� ��ȯ�Ͽ� mag�� ����

	Mat edge = mag > 150; // �Ӱ谪 �����Ͽ� ��� ���� ���� �Ӱ谪�� ���Ͽ� ����

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

	// �ּ� �Ӱ谪 50, �ִ� �Ӱ谪�� ���� 100, 150���� ����
	Mat dst1, dst2;
	Canny(src, dst1, 50, 100); // ����þ� ���͸� -> �׷����Ʈ ��� -> ���ִ� ���� -> ���� �Ӱ谪�� �̿��� �����׸��ý� ���� Ʈ��ŷ
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

	// ĳ�� ���� ���� ����� ����
	Mat edge;
	Canny(src, edge, 50, 150);

	// ������ ������ �Ķ���� p�� ��Ÿ ������ lines�� ����
	vector<Vec2f> lines; // �ڷ����� �ַ� Vec2f �Ǵ� Vec3f�� ���
	HoughLines(edge, lines, 1, CV_PI / 180, 250); // �Է� ����, ���� ������ ������ ��� ����, p���� �ػ�, ��Ÿ ���� �ػ�, �������� �Ǵ��� �Ӱ谪

	// �׷��̽����� edge ������ BGR 3ä�� �÷� �������� ��ȯ�Ͽ� ������
	Mat dst;
	cvtColor(edge, dst, COLOR_GRAY2BGR);

	// ���� ���� ������ �׸��� ���� �ݺ���
	for (size_t i = 0; i < lines.size(); i++) {
		float r = lines[i][0], t = lines[i][1]; // p�� ���� r��, ��Ÿ�� ���� t�� ����
		double cos_t = cos(t), sin_t = sin(t);
		double x0 = r * cos_t, y0 = r * sin_t;
		double alpha = 100; // ���� ����� ũ�� �����ؾ� �ڿ������� ������ �׸� �� �ִ�.

		// ���� ���� �� �� ��ǥ�� ���Ѵ�.
		Point pt1(cvRound(x0 + alpha * (-sin_t)), cvRound(y0 + alpha * cos_t));
		Point pt2(cvRound(x0 - alpha * (-sin_t)), cvRound(y0 - alpha * cos_t));
		line(dst, pt1, pt2, Scalar(0, 0, 255), 2, LINE_AA); // ���� �׸���
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

	// ĳ�� ���� ���� ��� ����
	Mat edge;
	Canny(src, edge, 50, 150);

	// ��� ���� ������ �������� ���� ��ǥ�� ����
	vector<Vec4i> lines;
	HoughLinesP(edge, lines, 1, CV_PI / 180, 160, 50, 5);

	// �÷� �������� ��ȯ�Ͽ� ����
	Mat dst;
	cvtColor(edge, dst, COLOR_GRAY2BGR);

	// ������ ��� ���� ����(��ǥ)�� ��� ���� ���� ������ �������� �׸���.
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

	// ������ ����
	Mat blurred;
	blur(src, blurred, Size(3, 3));

	// ���� ����, 
	// (�Է� ����, ��� ����, ����� default, ���� �迭�� �Է� ����� ����, �ּ� �Ӱ谪, �ִ� �Ӱ谪, ���� �迭 ���� ���� �Ӱ谪(�Ӱ谪 ���� ũ�� ���� �߽�����))
	vector<Vec3f> circles;
	HoughCircles(blurred, circles, HOUGH_GRADIENT, 1, 50, 150, 30);

	// �÷� �������� ��ȯ
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// �� �׸��� �ݺ���
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