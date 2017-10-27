#include "DatasetReader.h"

//-----------------------------------------------------------------------------------
//Class constructor
DatasetReader::DatasetReader(){

	//Image formats
	imgFmt.push_back("png");
	imgFmt.push_back("jpg");
	imgFmt.push_back("jpeg");
	imgFmt.push_back("bmp");

	//Initialize random seed
	srand(time(0));
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//Class destructor
DatasetReader::~DatasetReader(){}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//Dataset selection
int DatasetReader::selectDataset(){

	mainPathStr = DATASETS_PATH;
	vector<string> folders;
	int sel;	
	int ext;

	struct dirent *ent;
	DIR *dir;

	//Read datasets path
	strcpy_s(mainPathCh, mainPathStr.c_str());
	if ((dir = opendir(mainPathCh)) != NULL){

		//Loop through folder items
		while ((ent = readdir(dir)) != NULL){

			//Avoid "." and ".." files
			if (ent->d_namlen > 2){

				//Check file extension
				ext = checkExtension(ent->d_name);
				if (ext == 2)
					folders.push_back(ent->d_name);
			}
		}

		if (folders.size() > 0){

			//Print menu
			cout << "Select Dataset:" << endl << endl;
			displayMenu(folders);

			//Select dataset
			std::cin >> sel;
			mainPathStr.append(folders[sel - 1]);
			mainPathStr.append("/");
		}
		else{
			cout << "No datasets for testing!" << endl;
			return -1;
		}
	}
	else{
		cout << "Folder defined in Definitions.h for DATASETS_PATH does not exist!" << endl;
		return -1;
	}

	return 0;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//Gets training set according definitions
int DatasetReader::getTrainingSet(size_t nSubjects, size_t nSamples, vector<SubjectInfo> &subjects){

	int ext;
	int rdmImg;
	Mat subjIm;	
	vector<string> folders;
	int rdmFolder;
	SubjectInfo subject;
	struct dirent *ent;		
	DIR *dir;				

	//Read subjects folders
	strcpy_s(mainPathCh, mainPathStr.c_str());
	if ((dir = opendir(mainPathCh)) != NULL){

		//Loop through folder items
		while ((ent = readdir(dir)) != NULL){

			//Avoid "." and ".." files
			if (ent->d_namlen > 2){

				//Check file extension
				ext = checkExtension(ent->d_name);
				if (ext == 2)
					folders.push_back(ent->d_name);
			}
		}
	}

	//Check if there are enough subjects to test
	if (folders.size() < nSubjects){
		cout << "Not enough subjects to test on!";
		return -1;
	}		

	//Randomly select nSubjects from folders list
	for (size_t i = 0; i < nSubjects; i++){

		if (folders.size() != 1)
			rdmFolder = rand() % (folders.size() - 1);
		else
			rdmFolder = 0;

		subject.name = folders[rdmFolder];
		subjects.push_back(subject);
		folders.erase(folders.begin() + rdmFolder);
	}
	
	//Iterate through subject list
	string subjPathStr;
	char subjPathCh[250];
	vector<string> subjImgsNames;
	for (size_t i = 0; i < subjects.size(); i++){

		//Get subject folder complete path
		subjPathStr = mainPathStr + subjects[i].name + "/";
		strcpy_s(subjPathCh, subjPathStr.c_str());

		//Open subject directory
		if ((dir = opendir(subjPathCh)) != NULL){
			subjImgsNames.clear();

			//Loop through folder items
			while ((ent = readdir(dir)) != NULL){

				//Avoid "." and ".." files
				if (ent->d_namlen > 2){

					//Check file extension
					ext = checkExtension(ent->d_name);
					if (ext == 1){
						//Save images names
						subjImgsNames.push_back(ent->d_name);
					}
				}
			}

			//Check if there are enough images to test
			if (subjImgsNames.size() < nSamples){
				cout << "Not enough images for subject " + subjects[i].name + "!";
				return -1;
			}

			//Get nSamples random images for training and store in class vector info
			for (size_t j = 0; j < nSamples; j++){
				
				if (subjImgsNames.size() > 1)
					rdmImg = rand() % (subjImgsNames.size() - 1);
				else
					rdmImg = 0;
				subjIm = cv::imread(subjPathStr + subjImgsNames[rdmImg]);		
				rescaleImage(subjIm);				
				subjects[i].trainingImages.push_back(subjIm);
				subjImgsNames.erase(subjImgsNames.begin() + rdmImg);
			}

			//Get the left images for test
			for (size_t j = 0; j < subjImgsNames.size(); j++){
				subjIm = cv::imread(subjPathStr + subjImgsNames[j]);
				rescaleImage(subjIm);
				subjects[i].testImages.push_back(subjIm);
			}
		}
	}	

	return 0;
}
//-----------------------------------------------------------------------------------

void DatasetReader::getRandomImagesForTest(size_t numImgs, vector<SubjectInfo> subjects, vector<Mat> &testImages){

	struct dirent *ent;
	DIR *dir;
	vector<string> subjsNames;
	string subjectName;

	//Open subject directory
	if ((dir = opendir(mainPathCh)) != NULL){

		//Loop through folder items
		while ((ent = readdir(dir)) != NULL){

			//Avoid "." and ".." files
			if (ent->d_namlen > 2){

				//Check if subject is not part of the training set
				subjectName = ent->d_name;
				bool difName = true;
				for (size_t i = 0; i < subjects.size(); i++){
					if (subjectName == subjects[i].name){
						difName = false;
						break;
					}
				}
				//Store subject name if so
				if (difName)
					subjsNames.push_back(subjectName);
			}
		}
	}

	//Select numImgs random images for test
	int rdmSubj, rdmIm, imCount;
	string subjectPathStr;
	char subjectPathCh[250];
	Mat img;

	if (subjsNames.size() > 0){
		for (size_t i = 0; i < numImgs; i++){

			//Get random subject from set
			rdmSubj = rand() % (subjsNames.size() - 1);
			subjectPathStr = mainPathStr + "/" + subjsNames[rdmSubj];
			strcpy_s(subjectPathCh, subjectPathStr.c_str());

			//Random image selection idx
			rdmIm = rand() % subjects.size();
			imCount = 0;

			//Open subject directory
			if ((dir = opendir(subjectPathCh)) != NULL){

				//Loop through folder items
				while ((ent = readdir(dir)) != NULL){

					//Avoid "." and ".." files
					if (ent->d_namlen > 2){

						//Get random image rdmIm
						if (rdmIm == imCount){
							img = cv::imread(subjectPathStr + "/" + ent->d_name);
							if (!img.empty()){
								testImages.push_back(img.clone());
							}
							break;
						}
						imCount++;
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------------
//Checks the given file specified by "file" and return its category (image, video, 
//folder, other) 

int DatasetReader::checkExtension(string file)
{
	int ext = 0;

	//Loop through image formats
	for (size_t i = 0; i<imgFmt.size(); i++)
	{
		if (file.substr(file.find_last_of(".") + 1) == imgFmt[i])
		{
			ext = 1;
			return ext;
		}
	}

	//Check if folder
	size_t found = file.find(".");
	if (found == string::npos)
	{
		ext = 2;
		return ext;
	}

	return ext;
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//Displays the menu specified in "menu"
void DatasetReader::displayMenu(vector<string> menu)
{
	char kch[5];

	for (size_t i = 0; i<menu.size(); i++)
	{
		_itoa_s(i + 1, kch, 10);
		cout << kch << ") " << menu[i] << endl;
	}
}
//-----------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------
//Rescale image according definitions
void DatasetReader::rescaleImage(Mat &im){
	
	double maxArea = IM_RESIZE.area();
	double imArea = im.size().area();

	if (imArea > maxArea){
		double scaleFactor = sqrt(maxArea / imArea);
		int newRows = int(std::floor(scaleFactor * (double)im.rows));
		int newCols = int(std::floor(scaleFactor * (double)im.cols));
		resize(im, im, cv::Size(newCols, newRows), 0, 0, cv::INTER_CUBIC);
	}
}
//-----------------------------------------------------------------------------------

void DatasetReader::generateSubjectsCollage(vector<SubjectInfo> subjects){

	vector<Mat> trainingImgs;
	vector<Mat> subjectsSamples;
	Mat subjImgWithName;
	string subjName;
	cv::Point textPoint;

	for (size_t i = 0; i < subjects.size(); i++){

		trainingImgs = subjects[i].trainingImages;

		//Add name to images
		subjImgWithName = trainingImgs[trainingImgs.size() - 1].clone();
		textPoint = cv::Point((int)(0.05*subjImgWithName.cols), (int)(0.95*subjImgWithName.rows));
		cv::putText(subjImgWithName, subjects[i].name, textPoint, cv::FONT_HERSHEY_PLAIN, 2, cv::Scalar(255, 255, 255), 2);
		subjectsSamples.push_back(subjImgWithName);
	}
	
	size_t nRows, nCols;
	cv::Size collageSize;

	//Make collage
	findRowsAndColsForCollage(subjects.size(), subjectsSamples[0].size(), collageSize, nRows, nCols);
	Mat subjectsCollage(collageSize, CV_8UC3);
	tile(subjectsSamples, subjectsCollage, nCols, nRows);
	imshow("Subjects", subjectsCollage);
	cv::waitKey();
}

void DatasetReader::tile(const vector<Mat> &src, Mat &dst, int grid_x, int grid_y) {
	// patch size
	int width = dst.cols / grid_x;
	int height = dst.rows / grid_y;
	// iterate through grid
	int k = 0;
	for (int i = 0; i < grid_y; i++) {
		for (int j = 0; j < grid_x; j++) {
			Mat s = src[k++];
			resize(s, s, cv::Size(width, height));
			s.copyTo(dst(cv::Rect(j*width, i*height, width, height)));
		}
	}
}

void DatasetReader::findRowsAndColsForCollage(size_t numImgs, cv::Size imSize, cv::Size &collageSize, size_t &nRows, size_t &nCols){

	size_t maxCollageWidth = 720;

	//Find number of rows and columns for the collage
	for (size_t i = 4; i > 0; i--){
		if (numImgs % i == 0){
			nRows = numImgs / i;
			nCols = i;
			if (nRows > nCols){
				nCols = nRows;
				nRows = i;
			}
			break;
		}
	}

	//Find collage size
	collageSize.width = imSize.width * nCols;
	collageSize.height = imSize.height * nRows;

	if (collageSize.width > maxCollageWidth){

		double rsize = (double)maxCollageWidth / (double)collageSize.width;
		collageSize.height = (size_t)std::floor(rsize * collageSize.height);
		collageSize.width = maxCollageWidth;		
	}
}