#include "DatasetReader.h"
#include "FaceDetector.h"
#include "PersonRecognizer.h"
#include "Defintions.h"

void main(){

	//Initialize DatasetReader
	DatasetReader dr;

	//Initialize FaceDetector
	FaceDetector fd(string(CASCADE_PATH), DET_SCALE_FACTOR, DET_MIN_NEIGHBORS, DET_MIN_SIZE_RATIO, DET_MAX_SIZE_RATIO);

	//Initialize PersonRecognizer
	PersonRecognizer pr(LBPH_RADIUS, LBPH_NEIGHBORS, LBPH_GRID_X, LBPH_GRID_Y, LBPH_THRESHOLD);

	//Select Dataset
	int val = dr.selectDataset();
	if (val != 0){
		cout << "Enter any key to finish..." << endl;
		cin >> val;
		return;
	}		

	//Get training set
	vector<SubjectInfo> subjects;
	val = dr.getTrainingSet(NUM_CLASSES, NUM_SAMPLES, subjects);
	if (val != 0){
		cout << "Enter any key to finish..." << endl;
		cin >> val;
		return;
	}

	//Train model
	pr.trainModel(subjects, fd);

	//Show trained subjects collage
	dr.generateSubjectsCollage(subjects);

	//Test model
	pr.testModel(subjects, fd, dr);
}