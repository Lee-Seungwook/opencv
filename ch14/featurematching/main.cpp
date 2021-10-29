#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void corner_harris(); // 해리스 코너 검출
void corner_fast(); // FAST 코너 검출
void detect_keypoints(); // 키포인트 검출
void keypoint_matching(); // 키포인트 매칭
void good_matching(); // 키포인트 매칭 후 좋은 매칭 선별
void find_homography(); // 키포인트 매칭 및 호모그래피 계산
void stitching(int argc, char* argv[]); // 영상 이어 붙이기

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

	// 해리스 코너 응답 함수 행렬을 저장
	Mat harris;
	cornerHarris(src, harris, 3, 3, 0.04);

	// 코너 응답 함수 행렬을 0 ~ 255로 정규화하고 타입을 변경하여 영상으로 저장
	Mat harris_norm;
	normalize(harris, harris_norm, 0, 255, NORM_MINMAX, CV_8U);

	// 입력 영상을 트루 컬러 형식으로 변경
	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// harris_norm 영상에서 120보다 큰 픽셀을 코너로 검출
	for (int j = 1; j < harris.rows - 1; j++) {
		for (int i = 1; i < harris.cols - 1; i++) {
			if (harris_norm.at<uchar>(j, i) > 120) { // 비최대 억제를 수행, 주변 네 개의 픽셀을 비교하여 지역 최대인 경우에 영상에 표시
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

	// FAST 방법으로 코너 점을 검출, 임계값 60, 비최대 억제를 수행하도록 함
	vector<KeyPoint> keypoints;
	FAST(src, keypoints, 60, true);

	Mat dst;
	cvtColor(src, dst, COLOR_GRAY2BGR);

	// 검출된 모든 코너 점에 반지름이 5인 빨간색 원을 그림
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

	// ORB 클래스 객체를 생성하여 feature 스마트 포인터에 저장
	Ptr<Feature2D> feature = ORB::create();

	// ORB 키포인트를 검출하여 벡터에 저장
	vector<KeyPoint> keypoints;
	feature->detect(src, keypoints);
	// ORB 키포인트 기술자를 계산하여 행렬에 저장
	Mat desc;
	feature->compute(src, keypoints, desc);

	cout << "keypoints.size(): " << keypoints.size() << endl;
	cout << "desc.size(): " << desc.size() << endl;

	// 입력 영상에 키포인트를 그린 결과를 저장, 키포인트 위치, 크기, 방향 정보를 함께 나타내도록 설정
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

	// ORB 클래스 객체를 생성
	Ptr<Feature2D> feature = ORB::create();

	// 특징점을 검출하고 기술자를 계산하여 행렬에 저장
	vector<KeyPoint> keypoints1, keypoints2;
	Mat desc1, desc2;
	feature->detectAndCompute(src1, Mat(), keypoints1, desc1);
	feature->detectAndCompute(src2, Mat(), keypoints2, desc2);

	// BFMatcher 클래스 객체를 생성, 기술자 거리 계산 방식은 해밍 거리를 사용
	Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_HAMMING);

	// 기술자를 서로 매칭하여 결과를 저장
	vector<DMatch> matches;
	matcher->match(desc1, desc2, matches);

	// 매칭 결과를 이용하여 결과 영상 생성
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

	// ORB 클래스 객체를 생성
	Ptr<Feature2D> feature = ORB::create();

	// 특징점을 검출하고 기술자를 계산하여 행렬에 저장
	vector<KeyPoint> keypoints1, keypoints2;
	Mat desc1, desc2;
	feature->detectAndCompute(src1, Mat(), keypoints1, desc1);
	feature->detectAndCompute(src2, Mat(), keypoints2, desc2);

	// BFMatcher 클래스 객체를 생성, 기술자 거리 계산 방식은 해밍 거리를 사용
	Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_HAMMING);

	// 기술자를 서로 매칭하여 결과를 저장
	vector<DMatch> matches;
	matcher->match(desc1, desc2, matches);

	// 영상의 특징점 매칭 결과를 정렬
	sort(matches.begin(), matches.end());
	// 정렬된 매칭 결과에서 상위 50개 매칭 결과를 저장
	vector<DMatch> good_matches(matches.begin(), matches.begin() + 50);
	
	// 매칭되지 않은 특징점은 화면에 표시하지 않게하여 매칭 결과 영상 생성
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

	// ORB 클래스 객체를 생성
	Ptr<Feature2D> feature = ORB::create();

	// 특징점을 검출하고 기술자를 계산하여 행렬에 저장
	vector<KeyPoint> keypoints1, keypoints2;
	Mat desc1, desc2;
	feature->detectAndCompute(src1, Mat(), keypoints1, desc1);
	feature->detectAndCompute(src2, Mat(), keypoints2, desc2);

	// BFMatcher 클래스 객체를 생성, 기술자 거리 계산 방식은 해밍 거리를 사용
	Ptr<DescriptorMatcher> matcher = BFMatcher::create(NORM_HAMMING);

	// 기술자를 서로 매칭하여 결과를 저장
	vector<DMatch> matches;
	matcher->match(desc1, desc2, matches);

	// 영상의 특징점 매칭 결과를 정렬
	sort(matches.begin(), matches.end());
	// 정렬된 매칭 결과에서 상위 50개 매칭 결과를 저장
	vector<DMatch> good_matches(matches.begin(), matches.begin() + 50);

	// 매칭되지 않은 특징점은 화면에 표시하지 않게하여 매칭 결과 영상 생성
	Mat dst;
	drawMatches(src1, keypoints1, src2, keypoints2, good_matches, dst,
		Scalar::all(-1), Scalar::all(-1), vector<char>(),
		DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	// 매칭 결과에 저장된 영상과 훈련 영상의 특징점 좌표를 추출하여 저장
	vector<Point2f> pts1, pts2;
	for (size_t i = 0; i < good_matches.size(); i++) {
		pts1.push_back(keypoints1[good_matches[i].queryIdx].pt);
		pts2.push_back(keypoints2[good_matches[i].trainIdx].pt);
	}

	// 호모그래피 행렬을 구하여 H에 저장, RANSAC 알고리즘 사용
	Mat H = findHomography(pts1, pts2, RANSAC);

	// 입력 영상의 네 모서리 점을 저장한 후, 호모그래피 행렬 H를 이용하여 점들이 이동하는 위치를 계산하여 저장
	vector<Point2f> corners1, corners2;
	corners1.push_back(Point2f(0, 0));
	corners1.push_back(Point2f(src1.cols - 1.f, 0));
	corners1.push_back(Point2f(src1.cols - 1.f, src1.rows - 1.f));
	corners1.push_back(Point2f(0, src1.rows - 1.f));
	perspectiveTransform(corners1, corners2, H);

	// 점들이 위치하는 좌표를 저장
	vector<Point> corners_dst;
	for (Point2f pt : corners2) {
		corners_dst.push_back(Point(cvRound(pt.x + src1.cols), cvRound(pt.y)));
	}

	// 점들이 위치하는 좌표를 이용하여 녹색으로 사각형을 그림
	polylines(dst, corners_dst, true, Scalar(0, 255, 0), 2, LINE_AA);

	imshow("dst", dst);

	waitKey();
	destroyAllWindows();
}

// 명령 프롬프트로 실행해야 함
void stitching(int argc, char* argv[])
{
	// 명령행 인자 개수가 3보다 작으면 출력
	if (argc < 3) {
		cerr << "Usage:stitching.exe <image_file1> <image_file2> [<image_file3>...]" << endl;
		return;
	}

	// 명령행 인자로 전달된 영상 파일을 추가
	vector<Mat> imgs;
	for (int i = 1; i < argc; i++) {
		Mat img = imread(argv[i]);

		if (img.empty()) {
			cerr << "Image load failed!" << endl;
			return;
		}

		imgs.push_back(img);
	}

	// 객체 생성
	Ptr<Stitcher> stitcher = Stitcher::create();

	// 입력 영상을 이어 붙여 저장
	Mat dst;
	Stitcher::Status status = stitcher->stitch(imgs, dst);

	// 영상 이어붙이기가 실패했을 때
	if (status != Stitcher::Status::OK) {
		cerr << "Error on stitching!" << endl;
		return;
	}

	// 결과 영상을 파일로 저장
	imwrite("result.jpg", dst);

	imshow("dst", dst);

	waitKey();
	
}