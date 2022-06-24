#pragma once
//compile을 한번만 실행
//ㆍheader가 여러번 include 되는 것을 방지(≒#idndef) => 여러개의 cpp파일이 있을 때, 하나의 cpp파일이 수정되면, 그 파일만 컴파일하고 나머지는 x

// Qt
#include <QObject>

// opencv
#include <opencv2/opencv.hpp>
#include <QImage>

class QImage;
namespace cv 
{
	class Mat;
}

QImage mat_to_qimage_cpy(cv::Mat const &mat, bool swap = true);
QImage mat_to_qimage_ref(cv::Mat &mat, bool swap = true);
cv::Mat qimage_to_mat_cpy(QImage const &img, bool swap = true);
cv::Mat qimage_to_mat_ref(QImage &img, bool swap = true);

