#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Note binary archive could not used because of crash at loading phase.
//#include <boost/archive/binary_iarchive.hpp>
//#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/filesystem.hpp>

#include <opencv2/opencv.hpp>

#define DARK_MAT_FILE (std::string("./dark.dat"))
#define DARK_FLOAT_MAT_FILE (std::string("./dark_float.dat"))
#define DARK_TIFF_FILE (std::string("./dark.tif"))

BOOST_SERIALIZATION_SPLIT_FREE(cv::Mat)
namespace boost {
	namespace serialization {

		template<class Archive>
		void save(Archive & ar, const cv::Mat& m, const unsigned int version) {
			size_t elemSize = m.elemSize(), elemType = m.type();

			ar & m.cols;
			ar & m.rows;
			ar & elemSize;
			ar & elemType;
			size_t dataSize = m.cols * m.rows * m.elemSize();

			for (size_t dc = 0; dc < dataSize; ++dc) {
				ar & m.data[dc];
			}
		}

		template<class Archive>
		void load(Archive & ar, cv::Mat& m, const unsigned int version) {
			int cols, rows;
			size_t elemSize, elemType;

			ar & cols;
			ar & rows;
			ar & elemSize;
			ar & elemType;

			m.create(rows, cols, elemType);
			size_t dataSize = m.cols * m.rows * elemSize;

			for (size_t dc = 0; dc < dataSize; ++dc) {
				ar & m.data[dc];
			}
		}
	}
}

void saveMat(cv::Mat& m, std::string filename) {
	std::ofstream ofs(filename.c_str());
	//boost::archive::binary_oarchive oa(ofs);
	boost::archive::text_oarchive oa(ofs);
	oa << m;
}

void loadMat(cv::Mat& m, std::string filename) {
	std::ifstream ifs(filename.c_str());
	//boost::archive::binary_iarchive ia(ifs);
	boost::archive::text_iarchive ia(ifs);
	ia >> m;
}

bool is_file_exist(std::string pPath) {
	boost::filesystem::path p (pPath);
	
	if(boost::filesystem::exists(p) && boost::filesystem::is_regular_file(p))
		return true;
		
	return false;
}

bool list_files(std::string pPath, std::vector<std::string> &pFileList) {
	std::cout << "Search Path:" << pPath << std::endl;
	
	pFileList.clear();
	boost::filesystem::path p (pPath);

	boost::filesystem::directory_iterator end_itr;

	for (boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr)
	{
		if (boost::filesystem::is_regular_file(itr->path())) {
			std::string current_file = itr->path().string();
			pFileList.push_back(itr->path().string());
		}
	}

	return true;
}

const cv::String keys =
	"{help h usage ? |      | show help            }"
	"{@image         |      | input image file     }"
	"{@out_image     |      | output image file    }"
	"{@dark_dir      |      | dark image folder    }"
	"{F f float      |      | do float calculation }"
	"{U u usedark    |      | use dark mat if exist}"
	"{D d savedark   |      | save dark image      }"
	"{S s show       |      | show result          }"
	;

int main (int argc,char *argv[]) {	
    cv::CommandLineParser parser(argc, argv, keys);
    parser.about("A Simple dark frame subtraction program.");
	
	bool do_float = parser.has("F") ? true : false;
	bool use_darkmat = parser.has("U") ? true : false;
	bool save_dark = parser.has("D") ? true : false;
	bool do_show = parser.has("S") ? true : false;
	cv::String filename = parser.get<cv::String>("@image");
	cv::String outfilename = parser.get<cv::String>("@out_image");
	cv::String dark_dir = parser.get<cv::String>("@dark_dir");
	cv::String dark_mat_file = do_float ? cv::String(DARK_FLOAT_MAT_FILE) : cv::String(DARK_MAT_FILE);

	if (parser.has("h") || !parser.check() || filename.empty() || outfilename.empty() || 
		(!is_file_exist(dark_mat_file) && dark_dir.empty())) {
		parser.printMessage();
		return -1;
	}
	
	std::cout << "Processing file:" << filename << " float:" << do_float 
		<< " output file:" << outfilename << " show result:" << do_show
		<< " use dark mat:" << use_darkmat << " dark_dir:" << dark_dir << std::endl;
	
	cv::Mat image = cv::imread(filename);
	
	if(image.empty()) {
		std::cout << "Error: No input image file." << std::endl;
		return -1;
	}
	
	cv::Mat dark_image;
	cv::Mat mat_sum;
	std::vector<std::string> dark_image_list;
	
	if(is_file_exist(dark_mat_file) && use_darkmat) { // dark Mat file found.
		try {
			std::cout << "Importing " << dark_mat_file << "..." << std::endl;
			loadMat(mat_sum,dark_mat_file);
			
		} catch(...) {
			std::cout << "Error: dark mat file \"" << DARK_MAT_FILE << "\" read failed." << std::endl;
			return -1;
		}
	} else { // make dark Mat from dark image files.
		if(do_float)
			mat_sum = cv::Mat::zeros(image.size(),CV_32FC3);
		else
			mat_sum = cv::Mat::zeros(image.size(),CV_16UC3);
		
		try {
			list_files(dark_dir,dark_image_list);
			std::cout << "Reading dark image files..." << std::endl;
			cv::Mat buf_mat;
			
			for(int i = 0;i < dark_image_list.size();++i) {
				cv::Mat dark_image;
				std::cout << dark_image_list[i] << std::endl;
				dark_image = cv::imread(dark_image_list[i]);
				if(dark_image.empty()) throw 1;
				
				if(do_float) {
					dark_image.convertTo(buf_mat, CV_32FC3, 1.0/255.);
					mat_sum += buf_mat;
				} else {
					dark_image.convertTo(buf_mat, CV_16UC3, 1);
					mat_sum += buf_mat;
				}
			}
			
			if(do_float) {
				mat_sum /= (dark_image_list.size());
			} else {
				mat_sum /= (float)(dark_image_list.size());
			}
			if(use_darkmat) saveMat(mat_sum, dark_mat_file);
			
		} catch(...) {
			std::cout << "Error: failed to read dark image files." << std::endl;
			return -1;
		}
	} 
	
	if(do_float) {
		mat_sum.convertTo(dark_image, CV_8UC3, 255.);
	} else {
		mat_sum.convertTo(dark_image, CV_8UC3, 1.);
	}

	cv::Mat result_image = image - dark_image;
	if(save_dark) cv::imwrite(DARK_TIFF_FILE, dark_image);
	cv::imwrite(outfilename, result_image);
	
	if(do_show) {
		cv::namedWindow("original",cv::WINDOW_NORMAL);
		cv::imshow("original", image);
		cv::namedWindow("result",cv::WINDOW_NORMAL);
		cv::imshow("result", result_image);
		cv::namedWindow("dark",cv::WINDOW_NORMAL);
		cv::imshow("dark", dark_image);
		cv::waitKey(0);
	}
	
	return 0;
}
