#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void affine_transform(); // ������ ��ȯ
void affine_translation(); // �̵� ��ȯ
void affine_shear(); // ���� ��ȯ
void affine_scale(); // ũ�� ��ȯ
void affine_rotation(); // ȸ�� ��ȯ
void affine_flip(); // ��Ī ��ȯ

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

	// �� * ��� ������ ������ ��ȯ�� ���� �� �� ���� ��ǥ�� ����
	Point2f srcPts[3], dstPts[3];
	srcPts[0] = Point2f(0, 0);
	srcPts[1] = Point2f(src.cols - 1, 0);
	srcPts[2] = Point2f(src.cols - 1, src.rows - 1);
	dstPts[0] = Point2f(50, 50);
	dstPts[1] = Point2f(src.cols - 100, 100);
	dstPts[2] = Point2f(src.cols - 50, src.rows - 50);

	// 2 * 3 ������ ��ȯ ����� M�� ����
	Mat M = getAffineTransform(srcPts, dstPts);

	// ������ ��ȯ ��� M�� �̿��Ͽ�, �Է� ������ ������ ��ȯ�Ͽ� ��¿� ����, 
	// Size()�� ����Ͽ� ��� ������ �Է� ������ ũ��� ������ ��
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

	// ���η� 150�ȼ�, ���η� 100�ȼ� �̵��ϴ� ������ ��ȯ ��� M�� ����
	Mat M = Mat_<double>({ 2, 3 }, { 1, 0, 150, 0, 1, 100 });

	// �Է� ������ �̵� ��ȯ�Ͽ� ��� ������ ����
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
	resize(src, dst1, Size(), 4, 4, INTER_NEAREST); // �ֱٹ� ������
	resize(src, dst2, Size(1920, 1280)); // default�� �缱�� ������
	resize(src, dst3, Size(1920, 1280), 0, 0, INTER_CUBIC); // 3�� ȸ�� ������
	resize(src, dst4, Size(1920, 1280), 0, 0, INTER_LANCZOS4); // ���ʽ� ������

	// ��ǥ 400, 500 ���� 400 * 500 �������� ������ ���
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

	Point2f cp(src.cols / 2.f, src.rows / 2.f); // ������ �߽� ��ǥ�� ����Ű�� ����
	Mat M = getRotationMatrix2D(cp, 20, 1); // �ݽð� �������� 20�� ȸ���ϴ� ��ȯ ��� ����

	Mat dst;
	warpAffine(src, dst, M, Size()); // ������ ȸ����Ű�� ������ ��ȯ ����

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