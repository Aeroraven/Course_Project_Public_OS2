#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/imgproc/types_c.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <sstream>
#include <fstream>
using namespace cv;
using namespace std;

int main() {
	ofstream of;
	of.open("total.txt");
	for (int i = 0; i < 128; i++) {
		Mat* cvMat = new Mat(28, 18, CV_8UC3);
		//HersheyFonts
		stringstream ss;
		stringstream rs;
		stringstream ts;
		ss << (char)i;
		rs << i;
		ts << i;
		int baseline;
		int w = getTextSize(ss.str().c_str(), FONT_HERSHEY_TRIPLEX, 1, 1, &baseline).width;
		
		
		putText(*cvMat, ss.str().c_str(), { 13-w/2,17 }, FONT_HERSHEY_TRIPLEX, 0.65, { 0, 0, 0 }, 1);
		imshow("hello", *cvMat);
		rs << ".bmp";
		ts << ".txt";
		imwrite(rs.str().c_str(), *cvMat);
		cout << "Completed - " << i << endl;
		
		for (int j = 0; j < 28; j++) {
			for (int k = 0; k < 18; k++) {
				if (!(bool)(cvMat->at<Vec3b>(j, k)[0]) == 1) {
					of << "KRNL_VIDEO_CHARMAP[" << i << "][" << (j * 18 + k) << "]=";
					of << !(bool)(cvMat->at<Vec3b>(j, k)[0]);
					of <<";"<< endl;
				}
				
			}
			//of << endl;
		}
		
		delete cvMat;
	}
	of.close();
	return 0;
}