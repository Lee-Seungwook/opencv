#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

void color_inverse(); // Ʈ�� �÷� ���� ����
void color_grayscale(); // �׷��� ������ �������� ��ȯ
void color_split(); // Ʈ�� �÷� ���� ���� ������
void color_req(); // ������׷� �յ�ȭ

void color_inRange(); // Ư�� ���� ���� ����
void on_hue_changed(int, void*); // ���� ���� ������ ���� ��ü�� ����

int lower_hue = 40, upper_hue = 80; // �� ���� Ʈ���� ��ġ�� ������ ������ ����
Mat src, src_hsv, mask; // Mat ��ü

void color_backproj(); // ������׷� �������� �̿��� �Ǻλ� ���� ���� 




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

	// ���� ���� ����� ��ü ����
	Mat dst(src.rows, src.cols, src.type());

	// �Է� ���� �����Ͽ� ��� ���� ����
	for (int j = 0; j < src.rows; j++) {
		for (int i = 0; i < src.cols; i++) {
			// dst.at<Vec3b>(j, i) = Vec3b(255, 255, 255) - src.at<Vec3b>(j, i); // ���� ������ ����� ����
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

	// RGB ������ �׷��� ������ �������� ��ȯ
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

	// ���� ��� ������ ���Ϳ� ����
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

	// BGR �� ������ �Է� ������ YCrCb �� �������� ����
	Mat src_ycrcb;
	cvtColor(src, src_ycrcb, COLOR_BGR2YCrCb);

	// ������ ä���� �и��Ͽ� ����
	vector<Mat> ycrcb_planes;
	split(src_ycrcb, ycrcb_planes);

	// Y���п� �ش��ϴ� ���� ���ؼ��� ������׷� ��Ȱȭ ����
	equalizeHist(ycrcb_planes[0], ycrcb_planes[0]);

	// ���� �� ������ �ٽ� ��ħ
	Mat dst_ycrcb;
	merge(ycrcb_planes, dst_ycrcb);

	// BGR�������� ��ȯ
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

	// ������ HSV �������� ��ȯ�Ͽ� ����(src_hsv)
	cvtColor(src, src_hsv, COLOR_BGR2HSV);

	imshow("src", src);

	// ������ ���� ���� ���� ���� ������ �� �ִ� �� ���� Ʈ���ٸ� ����
	// ������ �ִ밪�� 179��, �ݹ� �Լ��� on_hue_changed �Լ��� ����
	namedWindow("mask");
	createTrackbar("Lower Hue", "mask", &lower_hue, 179, on_hue_changed);
	createTrackbar("Upper Hue", "mask", &upper_hue, 179, on_hue_changed);
	on_hue_changed(0, 0); // ó�� ����� �� ������ ���������� ��µǵ��� Ʈ���� �ݹ� �Լ��� ������ ȣ��

	waitKey();
}

void on_hue_changed(int, void *)
{
	// ����ڰ� ������ ������ ���� ���� ���� ���� �̿��Ͽ� ��ü�� ����
	// ä���� ������ 100 ~ 255, ���� ������ 0 ~ 255, ä���� ������ ����ڰ� �����Ͽ� ��� ������ ���
	Scalar lowerb(lower_hue, 100, 0);
	Scalar upperb(upper_hue, 255, 255);
	
	// HSV �� ������ ������ lowerb���� upperb ������ ��ġ�� �ȼ��� ������� ������ mask ������ ���� 
	inRange(src_hsv, lowerb, upperb, mask);

	imshow("mask", mask);
}

void color_backproj()
{
	Mat ref, ref_ycrcb, mask;
	ref = imread("ref.png", IMREAD_COLOR);
	mask = imread("mask.bmp", IMREAD_GRAYSCALE);
	
	// ���� ��ȯ
	cvtColor(ref, ref_ycrcb, COLOR_BGR2YCrCb);

	// ���� ���󿡼� �Ǻλ� ������ 2���� ������׷��� ����Ͽ� hist�� ����
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

	// ������ ���� hist�� ������׷��� �̿��Ͽ� �Է� ���󿡼� ������׷� �������� ����
	Mat backproj;
	calcBackProject(&src_ycrcb, 1, channels, hist, backproj, ranges, 1, true);

	imshow("src", src);
	imshow("backproj", backproj);
	waitKey();
}