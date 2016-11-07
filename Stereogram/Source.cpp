#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <string>
#include <cctype>

#include "ArgParser.h"

cv::RNG rng(51948);
void gen_pattern(cv::Mat &p){
	
	if (p.channels() == 1){
		for (int i = 0; i < p.size().height; i++){
			for (int j = 0; j < p.size().width; j++){
				p.at<uchar>(i, j) = rng.uniform(0, 256);
			}
		}
		cv::cvtColor(p, p, CV_GRAY2BGR);
	} else{
		for (int i = 0; i < p.size().height; i++){
			for (int j = 0; j < p.size().width; j++){
				uchar r = rng.uniform(0, 256);
				uchar g = rng.uniform(0, 256);
				uchar b = rng.uniform(0, 256);
				p.at<cv::Vec3b>(i, j) = cv::Vec3b(b, g, r);
			}
		}
	}
}

void createStereogram(cv::Mat &output, cv::Mat depth, cv::Mat pattern, int pattern_div = 8){
	output = cv::Mat::zeros(depth.size(), CV_8UC3);

	for (int i = 0; i < depth.size().width; i++){
		for (int j = 0; j < depth.size().height; j++){

			if (i < pattern.size().width){
				output.at<cv::Vec3b>(j, i) = pattern.at<cv::Vec3b>(j%pattern.size().height, i%pattern.size().width);
			}
			else{
				int shift = depth.at<uchar>(j, i) / (pattern_div / 2);
				output.at<cv::Vec3b>(j, i) = output.at<cv::Vec3b>(j, i - pattern.size().width + shift);
			}
		}
	}
}


void generate_Pic(std::string output, std::string depth, std::string pat){
	cv::Mat image = cv::imread(depth, CV_LOAD_IMAGE_GRAYSCALE);


	int pattern_div = 8;
	int pattern_width = image.size().width / pattern_div;
	

	cv::Mat pattern;
	if (pat == ""){
		pattern = cv::Mat::zeros(cv::Size(pattern_width, image.size().height), CV_8UC3);
		gen_pattern(pattern);
	} else{
		pattern = cv::imread(pat);
		int pattern_height = pattern_width * pattern.size().height / pattern.size().width;
		cv::resize(pattern, pattern, cv::Size(pattern_width, pattern_height));
	}


	cv::Mat outputMat;

	createStereogram(outputMat, image, pattern, pattern_div);

	cv::imwrite(output, outputMat);
}

void generate_Anime(std::string output, std::string depth, std::string pat){

	cv::VideoCapture cap(depth);
	cv::VideoWriter writer;

	cv::Mat image;
	cv::Mat outputMat;


	int pattern_div = 8;
	int pattern_width = image.size().width / pattern_div;

	cv::Mat pattern;
	if (pat == ""){
		pattern = cv::Mat::zeros(cv::Size(pattern_width, image.size().height), CV_8UC3);
		gen_pattern(pattern);
	}
	else{
		pattern = cv::imread(pat);
		int pattern_height = pattern_width * pattern.size().height / pattern.size().width;
		cv::resize(pattern, pattern, cv::Size(pattern_width, pattern_height));
	}


	while (1){
		if (!cap.isOpened()){
			std::cout << "error" << std::endl;
			break;
		}
		cap >> image;
		if (image.empty())break;

		cv::cvtColor(image, image, CV_BGR2GRAY);

		createStereogram(outputMat, image, pattern, pattern_div);

		if (!writer.isOpened()){
			writer.open(output, CV_FOURCC('W', 'M', 'V', '2'), 30, outputMat.size(), true);
		}

		writer << outputMat;
		//offset=rand.uniform(0, 1000);

	}

	writer.release();
}

int checkFileExt(std::string ext){
	std::transform(ext.begin(), ext.end(), ext.begin(), tolower);
	if (ext == "avi" || ext == "wmn" || ext == "mp4")return 1;
	else if (ext == "png" || ext == "jpg")return -1;
	else return 0;
}

int main(int argc, char *argv[]){

	std::string opts[] = {
		"-i", "-o", "-p"
	};

	

	if (argc <= 1){
		std::cout << "No arguments..." << std::endl;
		return 0;
	}

	std::vector<std::string> options(opts, opts + 3);

	ending::ArgParser parser(options, argc, argv);

	std::string inFile = parser.getString("-i", "");
	if (inFile == ""){
		std::cout << "No input file..." << std::endl;
		return 0;
	}

	std::string outFile = parser.getString("-o", std::string("x") + inFile);

	std::string patFile = parser.getString("-p", "");

	switch (checkFileExt(inFile.substr(inFile.rfind('.') + 1, std::string::npos))){
	case -1:
		generate_Pic(outFile, inFile, patFile);
		break;
	case 0:
		std::cout << "Invalid input type..." << std::endl;
		return 0;
	case 1:
		generate_Anime(outFile, inFile, patFile);
		break;
	}


	return 0;
}


/*
int main(void){
	cv::Mat image;

	cv::Mat output;

	std::cout << "input(0:pic / 1:film) : ";
	int select;
	std::cin >> select;

	std::cout << "input pattern(N/-): ";
	std::string pat;
	std::cin >> pat;

	cv::Mat pattern;

	if (pat != "N"){
		pattern = cv::imread(pat);
	}

	if (select == 0){

		std::string inFile;
		std::string outFile;

		std::cout << "Input file name: ";
		std::cin >> inFile;

		std::cout << "Input output name: ";
		std::cin >> outFile;

		image = cv::imread(inFile, CV_LOAD_IMAGE_GRAYSCALE);

		int offset = 0;
		if (pat != "N"){
			createStereogram(output, image, pattern, 8, offset);
		}
		else{
			createStereogram(output, image, 8);
		}

		cv::imwrite(outFile, output);
	}
	else if (select == 1){

		std::string inFile;
		std::string outFile;

		std::cout << "Input file name: ";
		std::cin >> inFile;

		std::cout << "Input output name: ";
		std::cin >> outFile;

		cv::VideoCapture cap(inFile);
		cv::VideoWriter writer;

		int offset = 0;

		cv::RNG rand(5647);

		while (1){
			if (!cap.isOpened()){
				std::cout << "error" << std::endl;
				break;
			}
			cap >> image;
			if (image.empty())break;

			cv::cvtColor(image, image, CV_BGR2GRAY);

			if (pat != "N"){
				createStereogram(output, image, pattern, 8, offset);
			}
			else{
				createStereogram(output, image, 8);
			}

			if (!writer.isOpened()){
				writer.open(outFile, CV_FOURCC('W', 'M', 'V', '2'), 30, output.size(), true);
			}

			writer << output;
			//offset=rand.uniform(0, 1000);

		}

		writer.release();
	}

	return 0;
	}*/