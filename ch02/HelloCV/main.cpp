#include "opencv2/opencv.hpp"
#include "iostream"

#include "ch04.h"

using namespace std;
using namespace cv;

int main()
{
	cout << "Hello OpenCV" << CV_VERSION << endl;



	Mat img;
	img = imread("lenna.bmp"); // �̹��� �ҷ�����

	if (img.empty()) { // ��ü�� �������(�����Ͱ� ���������� �ҷ��Դ��� �Ǵ�)
		cerr << "Image load failed!" << endl;
		return -1;
	}

	namedWindow("image"); // image��� �̸����� ������ ����� �� â�� ����

	//moveWindow("image", 20, 20); // ���� ��� â�� x, y ��ǥ�� �̵�
	//resizeWindow("image", 256, 256); // ���� ��� â�� ũ�⸦ ����

	imshow("image", img); // ������ ȭ�鿡 ���
	waitKey(); // Ű���� �Է��� �޴� �뵵�� ���, waitkey���� ȣ���ؾ� ȭ�鿡 ������ ��Ÿ��

	// jpeg ���Ϸ� ����� �����Ͽ� ����
	/*vector<int> params;
	params.push_back(IMWRITE_JPEG_QUALITY);
	params.push_back(95);
	imwrite("lenna.jpg", img, params);*/

	// ���� ����
	// imwrite("lennacopy.bmp", img);

	 destroyWindow("image"); // ���� ��� â�� �ݴ´�. 
	// destroyAllWindows(); // ��� ���� ���â�� �ݴ´�.
	
	//camera_in(); // ��ǻ�� ī�޶�
	
	// video_in(); // ���� ���

	// void camera_in_video_out(); // ī�޶�� ���� ������ ����
	return 0;
}
