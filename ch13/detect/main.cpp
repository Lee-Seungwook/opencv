#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;
using namespace cv;

void template_matching(); // ���ø� ��Ī
void detect_face(); // �� ����
void detect_eyes(); // �� ����
void detect_pedestrian(); // ȣ�� �˰����� ����� ������ ����
void decode_qrcode(); // qr �ڵ� ���� �� �ؼ�

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

	// �Է� ���� ��⸦ 50��ŭ ����
	img = img + Scalar(50, 50, 50);

	// �Է� ���� ǥ�� ������ 10�� ����þ� ���� �߰� ( ���� ȹ�� �������� �߻��� �� �ִ� ������ ������ ������ �ùķ��̼� �ϰ���)
	Mat noise(img.size(), CV_32SC3);
	randn(noise, 0, 10);
	add(img, noise, img, Mat(), CV_8UC3);

	Mat res, res_norm;
	matchTemplate(img, templ, res, TM_CCOEFF_NORMED); // ���ø� ��Ī ����
	normalize(res, res_norm, 0, 255, NORM_MINMAX, CV_8U); // ���� ���� 0 ~ 255 ���̷� ����ȭ�Ͽ� ����

	// ����� �ִ� ��ġ�� ã�� ����
	double maxv;
	Point maxloc;
	minMaxLoc(res, 0, &maxv, 0, &maxloc);
	cout << "maxv: " << maxv << endl;

	// ���ø� ��Ī���� ã�� ��ġ�� ������ �簢������ ǥ��
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
	
	// �� ���� xml ������ �ҷ���
	CascadeClassifier classifier("haarcascade_frontalface_default.xml");

	if (classifier.empty()) {
		cerr << "XML load failed!" << endl;
		return;
	}

	// ���� �����Ͽ� ����� �簢�� ������ ����
	vector<Rect> faces;
	classifier.detectMultiScale(src, faces);

	// ����� ���� �簢���� �Է� ���� ��������� �׸�
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

		Mat faceROI = src(face); // ������ �簢�� �� ������ �κ� ������ �����Ͽ� faceROI�� ����
		// ���� ����
		vector<Rect> eyes;
		eye_classifier.detectMultiScale(faceROI, eyes);

		// ������ ���� �߾ӿ� �Ķ��� ���� �׸�, �Է� ������ ������ ������ ����ϱ� ������ �Է� ���󿡵� ���� ǥ�ð� ��
		for (Rect eye : eyes) {
			Point center(eye.x + eye.width / 2, eye.y + eye.height / 2);
			circle(faceROI, center, eye.width / 2, Scalar(255, 0, 0), 2, LINE_AA);
		}
	}

	imshow("src", src);

	waitKey();
	destroyAllWindows();
}

// ����� ���� ������ ��� ������ ���� ������, ������ ���� ��������
void detect_pedestrian()
{
	VideoCapture cap("vtest.avi");

	if (!cap.isOpened()) {
		cerr << "Video open failed!" << endl;
		return;
	}

	// SVM �з��� ����� ���
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	Mat frame;
	while (true) {
		cap >> frame;
		if (frame.empty())
			break;

		// ������ �� �����Ӹ��� ������ ������ ����
		vector<Rect> detected;
		hog.detectMultiScale(frame, detected);

		// ����� �簢�� ������ �̿��Ͽ� ������ �������� 3�ȼ� �ΰ��� �簢���� �׸�
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

	// ������ QR�ڵ� ���� �� �ؼ��� �����Ͽ� ����
	vector<Point> points;
	String info = detector.detectAndDecode(src, points);

	// ������ ���������� �̷������ QR�ڵ忡 ������ �簢���� �׸���, �ؼ��� ���ڿ��� ȭ�� ���� ��ܿ� ������ ���ڷ� ���
	if (!info.empty()) {
		polylines(src, points, true, Scalar(0, 0, 255), 2);
		putText(src, info, Point(10, 30), FONT_HERSHEY_DUPLEX, 1, Scalar(0, 0, 255));
	}

	imshow("src", src);
	waitKey();
	destroyAllWindows();
}