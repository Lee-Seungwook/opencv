#include "opencv2/opencv.hpp"
#include "iostream"

using namespace std;
using namespace cv;

int main()
{
	cout << "Hello OpenCV" << CV_VERSION << endl;



	Mat img;
	img = imread("lenna.bmp"); // 이미지 불러오기

	if (img.empty()) { // 객체가 비었는지(데이터가 정상적으로 불러왔는지 판단)
		cerr << "Image load failed!" << endl;
		return -1;
	}

	namedWindow("image"); // image라는 이름으로 영상을 출력할 빈 창을 생성

	//moveWindow("image", 20, 20); // 영상 출력 창을 x, y 좌표로 이동
	//resizeWindow("image", 256, 256); // 영상 출력 창의 크기를 변경

	imshow("image", img); // 영상을 화면에 출력
	waitKey(); // 키보드 입력을 받는 용도로 사용, waitkey까지 호출해야 화면에 영상이 나타남

	// jpeg 파일로 압축률 지정하여 저장
	/*vector<int> params;
	params.push_back(IMWRITE_JPEG_QUALITY);
	params.push_back(95);
	imwrite("lenna.jpg", img, params);*/

	// 파일 저장
	// imwrite("lennacopy.bmp", img);

	// destroyWindow("image"); // 영상 출력 창을 닫는다. 
	// destroyAllWindows(); // 모든 영상 출력창을 닫는다.
	

	

	return 0;
}
