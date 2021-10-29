#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void corner_harris(); // �ظ��� �ڳ� ����
void corner_fast(); // FAST �ڳ� ����
void detect_keypoints(); // Ű����Ʈ ����
void keypoint_matching(); // Ű����Ʈ ��Ī
void good_matching(); // Ű����Ʈ ��Ī �� ���� ��Ī ����
void find_homography(); // Ű����Ʈ ��Ī �� ȣ��׷��� ���
void stitching(int argc, char* argv[]); // ���� �̾� ���̱�

int main(int argc, char* argv[])
{
	// corner_harris();
	// corner_fast();
	// detect_keypoints();
	// keypoint_matching();
	// good_matching();
	// find_homography();
	stitching(argc, argv);
}

void corner_harris()
{
	Mat src = imread("building.jpg", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// �ظ��� �ڳ� ���� �Լ� ����� ����
	Mat harris;
	cornerHarris(src, harris, 3, 3, 0.04);

	// �ڳ� ���� �Լ� ����� 0 ~ 255�� ����ȭ�ϰ� Ÿ���� �����Ͽ� �������� ����
	Mat harris_norm;
	normalize(harris, harris_norm, 0, 255, NORM_MINMAX, CV_8U);

	// �Է� ������ Ʈ�� �÷� �������� ����
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// harris_norm ���󿡼� 120���� ū �ȼ��� �ڳʷ� ����
	for (int j = 1; j < harris.rows - 1; j++) {
		for (int i = 1; i < harris.cols - 1; i++) {
			if (harris_norm.at<uchar>(j, i) > 120) { // ���ִ� ������ ����, �ֺ� �� ���� �ȼ��� ���Ͽ� ���� �ִ��� ��쿡 ���� ǥ��
				if (harris.at<float>(j, i) > harris.at<float>(j - 1, i) &&
					harris.at<float>(j, i) > harris.at<float>(j + 1, i) &&
					harris.at<float>(j, i) > harris.at<float>(j, i - 1) &&
					harris.at<float>(j, i) > harris.at<float>(j, i + 1)) {
					circle(dst, Point(i, j), 5, Scalar(0, 0, 255), 2);
				}
			}
		}
	}

	imshow("src", src);
	imshow("harris_norm", harris_norm);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void corner_fast()
{
	Mat src = imread("building.jpg", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// FAST ������� �ڳ� ���� ����, �Ӱ谪 60, ���ִ� ������ �����ϵ��� ��
	vector<KeyPoint> keypoints;
	FAST(src, keypoints, 60, true);

	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// ����� ��� �ڳ� ���� �������� 5�� ������ ���� �׸�
	for (KeyPoint kp : keypoints) {
		Point pt(cvRound(kp.pt.x), cvRound(kp.pt.y));
		circle(dst, pt, 5, Scalar(0, 0, 255), 2);
	}

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void detect_keypoints()
{
	Mat src = imread("box_in_scene.png", IMREAD_GRAYSCALE);

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// ORB Ŭ���� ��ü�� �����Ͽ� feature ����Ʈ �����Ϳ� ����
	Ptr<Feature2D> feature = ORB::create();

	// ORB Ű����Ʈ�� �����Ͽ� ���Ϳ� ����
	vector<KeyPoint> keypoints;
	feature->detect(src, keypoints);
	// ORB Ű����Ʈ ����ڸ� ����Ͽ� ��Ŀ� ����
	Mat desc;
	feature->compute(src, keypoints, desc);

	cout << "keypoints.size(): " << keypoints.size() << endl;
	cout << "desc.size(): " << desc.size() << endl;

	// �Է� ���� Ű����Ʈ�� �׸� ����� ����, Ű����Ʈ ��ġ, ũ��, ���� ������ �Բ� ��Ÿ������ ����
	Mat dst;
	drawKeypoints(src, keypoints, dst, Scalar::all(-1), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	imshow("src", src);
	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void keypoint_matching()
{
	Mat src1 = imread("box.png", IMREAD_GRAYSCALE);
	Mat src2 = imread("box_in_scene.png", IMREAD_GRAYSCALE);

	if (src1.empty() || src2.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// ORB Ŭ���� ��ü�� ����
	Ptr<Feature2D> feature = ORB::create();

	// Ư¡���� �����ϰ� ����ڸ� ����Ͽ� ��Ŀ� ����
	vector<KeyPoint> keypoints1, keypoints2;
	Mat desc1, desc2;
	feature->detectAndCompute(src1, Mat(), keypoints1, desc1);
	feature->detectAndCompute(src2, Mat(), keypoints2, desc2);

	// BFMatcher Ŭ���� ��ü�� ����, ����� �Ÿ� ��� ����� �ع� �Ÿ��� ���
	Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_HAMMING);

	// ����ڸ� ���� ��Ī�Ͽ� ����� ����
	vector<DMatch> matches;
	matcher->match(desc1, desc2, matches);

	// ��Ī ����� �̿��Ͽ� ��� ���� ����
	Mat dst;
	drawMatches(src1, keypoints1, src2, keypoints2, matches, dst);

	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void good_matching()
{
	Mat src1 = imread("box.png", IMREAD_GRAYSCALE);
	Mat src2 = imread("box_in_scene.png", IMREAD_GRAYSCALE);

	if (src1.empty() || src2.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// ORB Ŭ���� ��ü�� ����
	Ptr<Feature2D> feature = ORB::create();

	// Ư¡���� �����ϰ� ����ڸ� ����Ͽ� ��Ŀ� ����
	vector<KeyPoint> keypoints1, keypoints2;
	Mat desc1, desc2;
	feature->detectAndCompute(src1, Mat(), keypoints1, desc1);
	feature->detectAndCompute(src2, Mat(), keypoints2, desc2);

	// BFMatcher Ŭ���� ��ü�� ����, ����� �Ÿ� ��� ����� �ع� �Ÿ��� ���
	Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_HAMMING);

	// ����ڸ� ���� ��Ī�Ͽ� ����� ����
	vector<DMatch> matches;
	matcher->match(desc1, desc2, matches);

	// ������ Ư¡�� ��Ī ����� ����
	sort(matches.begin(), matches.end());
	// ���ĵ� ��Ī ������� ���� 50�� ��Ī ����� ����
	vector<DMatch> good_matches(matches.begin(), matches.begin() + 50);
	
	// ��Ī���� ���� Ư¡���� ȭ�鿡 ǥ������ �ʰ��Ͽ� ��Ī ��� ���� ����
	Mat dst;
	drawMatches(src1, keypoints1, src2, keypoints2, good_matches, dst,
		Scalar::all(-1), Scalar::all(-1), vector<char>(),
		DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

void find_homography()
{
	Mat src1 = imread("box.png", IMREAD_GRAYSCALE);
	Mat src2 = imread("box_in_scene.png", IMREAD_GRAYSCALE);

	if (src1.empty() || src2.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// ORB Ŭ���� ��ü�� ����
	Ptr<Feature2D> feature = ORB::create();

	// Ư¡���� �����ϰ� ����ڸ� ����Ͽ� ��Ŀ� ����
	vector<KeyPoint> keypoints1, keypoints2;
	Mat desc1, desc2;
	feature->detectAndCompute(src1, Mat(), keypoints1, desc1);
	feature->detectAndCompute(src2, Mat(), keypoints2, desc2);

	// BFMatcher Ŭ���� ��ü�� ����, ����� �Ÿ� ��� ����� �ع� �Ÿ��� ���
	Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_HAMMING);

	// ����ڸ� ���� ��Ī�Ͽ� ����� ����
	vector<DMatch> matches;
	matcher->match(desc1, desc2, matches);

	// ������ Ư¡�� ��Ī ����� ����
	sort(matches.begin(), matches.end());
	// ���ĵ� ��Ī ������� ���� 50�� ��Ī ����� ����
	vector<DMatch> good_matches(matches.begin(), matches.begin() + 50);

	// ��Ī���� ���� Ư¡���� ȭ�鿡 ǥ������ �ʰ��Ͽ� ��Ī ��� ���� ����
	Mat dst;
	drawMatches(src1, keypoints1, src2, keypoints2, good_matches, dst,
		Scalar::all(-1), Scalar::all(-1), vector<char>(),
		DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	// ��Ī ����� ����� ����� �Ʒ� ������ Ư¡�� ��ǥ�� �����Ͽ� ����
	vector<Point2f> pts1, pts2;
	for (size_t i = 0; i < good_matches.size(); i++) {
		pts1.push_back(keypoints1[good_matches[i].queryIdx].pt);
		pts2.push_back(keypoints2[good_matches[i].trainIdx].pt);
	}

	// ȣ��׷��� ����� ���Ͽ� H�� ����, RANSAC �˰��� ���
	Mat H = findHomography(pts1, pts2, RANSAC);

	// �Է� ������ �� �𼭸� ���� ������ ��, ȣ��׷��� ��� H�� �̿��Ͽ� ������ �̵��ϴ� ��ġ�� ����Ͽ� ����
	vector<Point2f> corners1, corners2;
	corners1.push_back(Point2f(0, 0));
	corners1.push_back(Point2f(src1.cols - 1.f, 0));
	corners1.push_back(Point2f(src1.cols - 1.f, src1.rows - 1.f));
	corners1.push_back(Point2f(0, src1.rows - 1.f));
	perspectiveTransform(corners1, corners2, H);

	// ������ ��ġ�ϴ� ��ǥ�� ����
	vector<Point> corners_dst;
	for (Point2f pt : corners2) {
		corners_dst.push_back(Point(cvRound(pt.x + src1.cols), cvRound(pt.y)));
	}

	// ������ ��ġ�ϴ� ��ǥ�� �̿��Ͽ� ������� �簢���� �׸�
	polylines(dst, corners_dst, true, Scalar(0, 255, 0), 2, LINE_AA);

	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

// ��� ������Ʈ�� �����ؾ� ��
void stitching(int argc, char* argv[])
{
	// ����� ���� ������ 3���� ������ ���
	if (argc < 3) {
		cerr << "Usage:stitching.exe <image_file1> <image_file2> [<image_file3>...]" << endl;
		return;
	}

	// ����� ���ڷ� ���޵� ���� ������ �߰�
	vector<Mat> imgs;
	for (int i = 1; i < argc; i++) {
		Mat img = imread(argv[i]);

		if (img.empty()) {
			cerr << "Image load failed!" << endl;
			return;
		}

		imgs.push_back(img);
	}

	// ��ü ����
	Ptr<Stitcher> stitcher = Stitcher::create();

	// �Է� ������ �̾� �ٿ� ����
	Mat dst;
	Stitcher::Status status = stitcher->stitch(imgs, dst);

	// ���� �̾���̱Ⱑ �������� ��
	if (status != Stitcher::Status::OK) {
		cerr << "Error on stitching!" << endl;
		return;
	}

	// ��� ������ ���Ϸ� ����
	imwrite("result.jpg", dst);

	imshow("dst", dst);

	waitKey();
	
}