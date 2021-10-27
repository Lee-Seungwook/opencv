#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void labeling_basic(); // �󺧸� �۵� ����
void labeling_stats(); // �󺧸��� �̿��Ͽ� ��ü�� �ٿ�� �ڽ� �׸���
void contours_basic(); // �ܰ��� ����� �׸���
void contours_hier(); // ���� ������ ����ϴ� �ܰ��� ����� �׸���

void contours_labeling(int argc, char* argv[]); // �ٰ��� ���� �� �ν�
void setLabel(Mat& img, const vector<Point>& pts, const String& label); // �󺧸� ����

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
	// �׷��� ������ ���� ����
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

	// data ����� ��� ���ҿ� 255�� ���� ����� ����
	Mat src = Mat(8, 8, CV_8UC1, data) * 255;

	// ���̺� ���� �����Ͽ� ����
	Mat labels;
	int cnt = connectedComponents(src, labels); // ������ ���̺� �� ��ü ���� ����

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

	// ���� �˰����� �̿��Ͽ� �Է� ������ ����ȭ �۾� ����
	Mat bin;
	threshold(src, bin, 0, 255, THRESH_BINARY | THRESH_OTSU);

	// ����ȭ ���� ���� ���̺��� �����ϰ� �� ��ü ������ ��� ������ ����
	Mat labels, stats, centroids;
	int cnt = connectedComponentsWithStats(bin, labels, stats, centroids);

	// �Է� ������ �÷��� ��ȯ
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// ��� ��ü ������ ���ؼ��� for �ݺ��� ����
	for (int i = 0; i < cnt; i++) {
		int* p = stats.ptr<int>(i);

		// ��ü�� �ȼ� ������ 20 ���� ������ �����̶�� �����Ͽ� ����
		if (p[4] < 20) continue;

		// ����� �簢���� �׸���
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
	Mat src = imread("contours.bmp", IMREAD_GRAYSCALE); // �ش� ������ ����ȭ �۾��� ����� �����̹Ƿ�, �ٸ� ������ ����� ��� ����ȭ �۾��� ���� �߰� �Ǿ�� �ҵ� ��

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// ��� �ܰ��� ����, ���� ������ �������� �ʴ´�.
	vector<vector<Point>> contours;
	findContours(src, contours, RETR_LIST, CHAIN_APPROX_NONE);

	// �÷� �������� ��ȯ
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// contours�� ����� ������ �ܰ����� ������ �������� �׷��ش�.
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

	// �ܰ��� ���� �� ���� ������ �޾ƿ� (RETR_CCOMP�� ����Ͽ� 2���� �������� ����)
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(src, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

	// �÷� �������� ��ȯ
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// ���� ������ ������� �ܰ��� ���θ� ����� ä�� (���� �β��� -1�̱� ������)
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

	// ���� �˰����� �̿��� ����ȭ �۾� ����
	Mat bin;
	threshold(gray, bin, 200, 255, THRESH_BINARY_INV | THRESH_OTSU);

	// �ܰ��� ���� ����
	vector<vector<Point>> contours;
	findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);

	// �� ��ü�� �ܰ��� ��ǥ�� pts ������ ����
	for (vector<Point>& pts : contours) {
		// ������ 400���� ������ �������� ����
		if (contourArea(pts) < 400)
			continue;

		// �ܰ����� �ٻ�ȭ�Ͽ� ����
		vector<Point> approx;
		approxPolyDP(pts, approx, arcLength(pts, true)*0.02, true);

		// �ܰ��� ���� ������ ����
		int vtc = (int)approx.size();

		// �������� ������ ����
		if (vtc == 3) {
			setLabel(img, pts, "TRI");
		}
		else if (vtc == 4) {
			setLabel(img, pts, "RECT");
		}
		// ���� �� ���� ������ ����
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
	// �ܰ��� �ֺ��� �ٿ�� �ڽ��� �׸��� label ���ڿ��� ���
	Rect rc = boundingRect(pts);
	rectangle(img, rc, Scalar(0, 0, 255), 1); // �ٿ�� �ڽ��� ��Ȳ������ ǥ��
	putText(img, label, rc.tl(), FONT_HERSHEY_PLAIN, 1, Scalar(0, 0, 255)); // �ڽ� ���� ��ܿ� ���ڿ��� ���
}