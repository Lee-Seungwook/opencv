#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void template_matching(); // 템플릿 매칭
void detect_face(); // 얼굴 검출
void detect_eyes(); // 눈 검출
void detect_pedestrian(); // 호그 알고리즘을 사용한 보행자 검출
void decode_qrcode(); // qr 코드 검출 및 해석

int main()
{
	// template_matching();
	// detect_face();
	// detect_eyes();
	// detect_pedestrian();
	decode_qrcode();
}

void template_matching()
{
	Mat img = imread("circuit.bmp", IMREAD_COLOR);
	Mat templ = imread("crystal.bmp", IMREAD_COLOR);

	if (img.empty() || templ.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	// 입력 영상 밝기를 50만큼 증가
	img = img + Scalar(50, 50, 50);

	// 입력 영상에 표준 편차가 10인 가우시안 잡음 추가 ( 영상 획득 과정에서 발생할 수 있는 잡음과 조명의 영향을 시뮬레이션 하고자)
	Mat noise(img.size(), CV_32SC3);
	randn(noise, 0, 10);
	add(img, noise, img, Mat(), CV_8UC3);

	Mat res, res_norm;
	matchTemplate(img, templ, res, TM_CCOEFF_NORMED); // 템플릿 매칭 수행
	normalize(res, res_norm, 0, 255, NORM_MINMAX, CV_8U); // 원소 값을 0 ~ 255 사이로 정규화하여 저장

	// 행렬의 최댓값 위치를 찾아 저장
	double maxv;
	Point maxloc;
	minMaxLoc(res, 0, &maxv, 0, &maxloc);
	cout << "maxv: " << maxv << endl;

	// 템플릿 매칭으로 찾은 위치를 빨간색 사각형으로 표시
	rectangle(img, Rect(maxloc.x, maxloc.y, templ.cols, templ.rows), Scalar(0, 0, 255), 2);

	imshow("templ", templ);
	imshow("res_norm", res_norm);
	imshow("img", img);

	waitKey();
	destroyAllWindows();
}

void detect_face()
{
	Mat src = imread("jisoo1.bmp");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}
	
	// 얼굴 검출 xml 파일을 불러옴
	CascadeClassifier classifier("haarcascade_frontalface_default.xml");

	if (classifier.empty()) {
		cerr << "XML load failed!" << endl;
		return;
	}

	// 얼굴을 검출하여 검출된 사각형 정보를 저장
	vector<Rect> faces;
	classifier.detectMultiScale(src, faces);

	// 검출된 영역 사각형을 입력 영상에 보라색으로 그림
	for (Rect rc : faces) {
		rectangle(src, rc, Scalar(255, 0, 255), 2);
	}

	imshow("src", src);

	waitKey();
	destroyAllWindows();
}

void detect_eyes()
{
	Mat src = imread("jisoo1.bmp");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	CascadeClassifier face_classifier("haarcascade_frontalface_default.xml");
	CascadeClassifier eye_classifier("haarcascade_eye.xml");

	if (face_classifier.empty() || eye_classifier.empty()) {
		cerr << "XML load failed!" << endl;
		return;
	}

	vector<Rect> faces;
	face_classifier.detectMultiScale(src, faces);

	for (Rect face : faces) {
		rectangle(src, face, Scalar(255, 0, 255), 2);

		Mat faceROI = src(face); // 검출한 사각형 얼굴 영역의 부분 영상을 추출하여 faceROI에 저장
		// 눈을 검출
		vector<Rect> eyes;
		eye_classifier.detectMultiScale(faceROI, eyes);

		// 검출한 눈의 중앙에 파란색 원을 그림, 입력 영상을 참조한 영상을 사용하기 때문에 입력 영상에도 원이 표시가 됨
		for (Rect eye : eyes) {
			Point center(eye.x + eye.width / 2, eye.y + eye.height / 2);
			circle(faceROI, center, eye.width / 2, Scalar(255, 0, 0), 2, LINE_AA);
		}
	}

	imshow("src", src);

	waitKey();
	destroyAllWindows();
}

// 디버그 모드로 실행할 경우 연산이 많아 느리다, 릴리즈 모드로 실행하자
void detect_pedestrian()
{
	VideoCapture cap("vtest.avi");

	if (!cap.isOpened()) {
		cerr << "Video open failed!" << endl;
		return;
	}

	// SVM 분류기 계수를 등록
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	Mat frame;
	while (true) {
		cap >> frame;
		if (frame.empty())
			break;

		// 동영상 매 프레임마다 보행자 검출을 수행
		vector<Rect> detected;
		hog.detectMultiScale(frame, detected);

		// 검출된 사각형 정보를 이용하여 임의의 색상으로 3픽셀 두게의 사각형을 그림
		for (Rect r : detected) {
			Scalar c = Scalar(rand() % 256, rand() % 256, rand() & 256);
			rectangle(frame, r, c, 3);
		}

		imshow("frame", frame);

		if (waitKey(10) == 27)
			break;
	}
}

void decode_qrcode()
{
	Mat src = imread("qrcode.png");

	if (src.empty()) {
		cerr << "Image load failed!" << endl;
		return;
	}

	QRCodeDetector detector;

	// 영상의 QR코드 검출 및 해석을 수행하여 저장
	vector<Point> points;
	String info = detector.detectAndDecode(src, points);

	// 검출이 정상적으로 이루어지면 QR코드에 빨간색 사각형을 그리고, 해석된 문자열을 화면 좌측 상단에 빨간색 글자로 출력
	if (!info.empty()) {
		polylines(src, points, true, Scalar(0, 0, 255), 2);
		putText(src, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
	}

	imshow("src", src);
	waitKey();
	destroyAllWindows();
}