#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

/** Paths: **/
#define CASCADE_PATH  "cascades/haarcascade_frontalface_default.xml"
#define DATASETS_PATH "Datasets/"

/** Face detector: **/
#define DET_SCALE_FACTOR   1.01
#define DET_MIN_NEIGHBORS  45
#define DET_MIN_SIZE_RATIO 0.5
#define DET_MAX_SIZE_RATIO 0.9

/** LBPH face recognizer: **/
#define LBPH_RADIUS    3
#define LBPH_NEIGHBORS 8
#define LBPH_GRID_X    8
#define LBPH_GRID_Y    8
#define LBPH_THRESHOLD 180.0
#define IDENTIFICATION_THRESHOLD 52

/** Classification: **/
#define NUM_CLASSES 9
#define NUM_SAMPLES 5

/** Image transformation: **/
#define IM_RESIZE cv::Size(300,300)

/** Face preprocessing: **/
#define IM_CROP 0.1
#define FACE_SIZE cv::Size(80,80)
#define GAMMA_VALUE 0.5

/** Training subject structure: **/
struct SubjectInfo{
	std::string name;
	std::vector<cv::Mat> trainingImages;
	std::vector<cv::Mat> testImages;
};
#endif