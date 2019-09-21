#include <stdio.h>
#include <iostream>
#include <vector>
#include <time.h>

#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/xfeatures2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/calib3d.hpp"

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

Mat surf_detection(Mat img_1, Mat img_2, Mat imgObject); /** @function main */

Mat objectDescriptor(Mat imgObject);
void saveToFile();

std::vector<KeyPoint> keyPointsObject;

int largestArea = 0;
int largestContourIndex = 0;
Rect boundRect;

RNG rng(12345);
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;
Mat imgHSV;
Mat imgThresholded;

int main(int argc, char** argv)
{
	// @TODO: REPLACE THE NEXT LINE WITH YOUR VIDEO URL
	VideoCapture cap("YOUR_FILE_NAME");

	if (!cap.isOpened())
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	int iLowH = 65;
	int iHighH = 180;

	int iLowS = 150;
	int iHighS = 255;

	int iLowV = 60;
	int iHighV = 255;

	int iLastX = -1;
	int iLastY = -1;

	Mat imgTmp;
	cap.read(imgTmp);

	Mat imgObject = imread("C:\\objectStop.png", CV_LOAD_IMAGE_GRAYSCALE);
	Mat objectDescript = objectDescriptor(imgObject);

	clock_t init, final;

	int index = 0;
	string filePathOrg = "D:\\Original\\Original_.jpg";
	string filePathHSV = "D:\\HSV\\HSV_.jpg";
	string filePathCnt = "D:\\Cnt\\Cnt_.jpg";
	string filePathFtr = "D:\\Ftr\\Ftr_.jpg";

	int length;

	while (true)
	{
		init = clock();
		Mat imgOriginal;

		bool bSuccess = cap.read(imgOriginal);

		if (!bSuccess)
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV);

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		Mat dst(imgOriginal.rows, imgOriginal.cols, CV_32FC1, Scalar::all(0));

		findContours(imgThresholded, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_TC89_KCOS);

		for (int i = 0; i < contours.size(); i++)
		{
			double item = contourArea(contours[i], false);

			if (8000 < item && item < 50000)
			{
				boundRect = boundingRect(contours[i]);

				Scalar color(255, 255, 255);
				drawContours(dst, contours, i, color, CV_FILLED, 8, hierarchy);
				rectangle(imgOriginal, boundRect, Scalar(0, 255, 0), 1, 8, 0);

				Mat gray = imgOriginal(boundRect);
				cvtColor(gray, gray, COLOR_BGR2GRAY);

				Mat out = surf_detection(objectDescript, gray, imgObject);

				imshow("Feature Detection", out);

			}
		}

		imshow("Original Contour Selected", imgOriginal);

		if (waitKey(1) == 27)
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
		index++;
		final = clock() - init;

		cout << "Iteration: " << (double)final / ((double)CLOCKS_PER_SEC) << endl;
	}
	return 0;
}

Mat objectDescriptor(Mat imgObject)
{
	Ptr <SURF> detector = SURF::create(400);
	Ptr <SURF> extractor = SURF::create();

	detector->detect(imgObject, keyPointsObject);

	Mat descriptorObject;
	extractor->compute(imgObject, keyPointsObject, descriptorObject);

	return descriptorObject;
}

Mat surf_detection(Mat descriptors_object, Mat img_scene, Mat img_object)
{
	Mat img_matches;
	Ptr <SURF> detector = SURF::create(400);
	std::vector<KeyPoint> keypoints_scene;
	detector->detect(img_scene, keypoints_scene);
	Ptr <SURF> extractor = SURF::create();
	Mat descriptors_scene;
	extractor->compute(img_scene, keypoints_scene, descriptors_scene);
	std::vector<DMatch> matches;
	BFMatcher matcher = BFMatcher(NORM_L2, false);
	matcher.match(descriptors_object, descriptors_scene, matches);
	double max_dist = 0; double min_dist = 100;
  for (int i = 0; i < descriptors_object.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	std::vector< DMatch > good_matches;

	for (int i = 0; i < descriptors_object.rows; i++)
	{
		if (matches[i].distance < 3 * min_dist)
		{
			good_matches.push_back(matches[i]);
		}
	}

	drawMatches(img_object, keyPointsObject, img_scene, keypoints_scene,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	std::vector<Point2f> obj;
	std::vector<Point2f> scene;

	for (int i = 0; i < good_matches.size(); i++)
	{
		obj.push_back(keyPointsObject[good_matches[i].queryIdx].pt);
		scene.push_back(keypoints_scene[good_matches[i].trainIdx].pt);
	}

	Mat H = findHomography(obj, scene, CV_RANSAC);

	std::vector<Point2f> obj_corners(4);
	obj_corners[0] = cvPoint(0, 0);
	obj_corners[1] = cvPoint(img_object.cols, 0);
	obj_corners[2] = cvPoint(img_object.cols, img_object.rows);
	obj_corners[3] = cvPoint(0, img_object.rows);
	std::vector<Point2f> scene_corners(4);

	return img_matches;
}

void saveToFile()
{
  std::cout << "Save to file wasn't defined during compiling.\nTo define save to file, plea";
  return 0;
}
