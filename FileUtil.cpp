#include "FileUtil.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <random>

namespace fs = std::filesystem;

namespace FileUtil {

std::vector<std::string> getFilesInDirectory(const std::string& directory_name){
	std::vector<std::string> filenames;
	fs::path directory(directory_name);

	fs::directory_iterator end_iterator;

	if(!fs::exists(directory)){
		throw std::runtime_error(directory_name + " does not exist");
	}
	if(!fs::is_directory(directory)){
		throw std::runtime_error(directory_name + " is not a directory");
	}

	for(fs::directory_iterator it(directory); it!=end_iterator; it++){
		if(fs::is_regular_file(it->path())){
			filenames.push_back(it->path().filename().string());
		}
	}
	std::sort(filenames.begin(), filenames.end());
	return filenames;
}

std::vector<std::string> getDirectoriesInDirectory(const std::string& directory_name){
	std::vector<std::string> filenames;
	fs::path directory(directory_name);

	fs::directory_iterator end_iterator;

	if(!fs::exists(directory)){
		throw std::runtime_error(directory_name + " does not exist");
	}
	if(!fs::is_directory(directory)){
		throw std::runtime_error(directory_name + " is not a directory");
	}

	for(fs::directory_iterator it(directory); it!=end_iterator; it++){
		if(fs::is_directory(it->path())){
			filenames.push_back(it->path().filename().string());
		}
	}
	return filenames;
}

std::string extractFileExtension(const std::string& filename){
	for(int i=filename.size()-1; i>=0; i--){
		if(filename[i] == '.'){
			return filename.substr(i+1);
		}
	}
	return "";
}

std::string getFileContents(const std::string& filename){
	if(!fileExists(filename)){
		throw std::runtime_error("No such file "+filename);
	}
	std::ifstream fp(filename, std::ios::binary|std::ios::in);
	std::string contents;
	std::stringstream buffer;
	if(fp.fail()){
		throw std::runtime_error("Could not open file "+filename);
	}
	buffer << fp.rdbuf();
	contents = buffer.str();
	fp.close();
	return contents;
}

void setFileContents(const std::string& path, const std::string& contents){
	const fs::path fp = path;
	if(fs::exists(fp)){
		if(!fs::is_regular_file(fp)){
			throw std::runtime_error("Entity "+path+" exists, but is not a regular file. Refusing to overwrite.");
		}
	}
	std::ofstream fp2(path, std::ios::binary|std::ios::out);
	if(fp2.bad()){
		throw std::runtime_error("Could not create file at "+path);
	}
	fp2 << contents;
	fp2.close();
}

std::string createTemporaryFile(const std::string& contents)
{
	std::random_device seed;
	std::default_random_engine rand_engine(seed());
	std::uniform_int_distribution<std::uint64_t>distribute(1,2UL<<63UL);
	std::string path_name = "/tmp/futil.temp."+std::to_string(distribute(rand_engine));
	setFileContents(path_name, contents);
	return path_name;
}

void ensureDirectoryExists(const std::string& path){
	const fs::path directory = path;
	if(fs::exists(directory)){
		if(fs::is_directory(directory)){
			// all good
			return;
		}
		throw std::runtime_error("Entity "+path+" exists, but is not a directory. Refusing to overwrite.");
	}else{
		if(!fs::create_directories(directory)){
			throw std::runtime_error("Could not create directory at "+path);
		}
	}
}

void ensureFileExists(const std::string& path){
	const fs::path fp = path;
	if(fs::exists(fp)){
		if(!fs::is_regular_file(fp)){
			throw std::runtime_error("Entity "+path+" exists, but is not a regular file. Refusing to overwrite.");
		}
	}else{
		std::ofstream fp2(path);
		if(fp2.bad()){
			throw std::runtime_error("Could not create file at "+path);
		}
		fp2.close();
	}
}

bool fileExists(const std::string& path){
	const fs::path fp = path;
	if(fs::exists(fp)){
		if(!fs::is_regular_file(fp)){
			throw std::runtime_error("Entity "+path+" exists, but is not a regular file");
		}
		return true;
	}
	return false;
}

void ensureFileDoesNotExist(const std::string& path){
	const fs::path fp = path;
	if(fs::exists(fp)){
		// Destroy it >:(
		if(!fs::is_regular_file(fp)){
			throw std::runtime_error("Can't destroy file "+path+" because it is a non-file entity");
		}
		fs::remove(fp);
	}

	// Work is done for you
}
	

void destroyFile(const std::string& path){
	if(!fs::exists(fs::path(path))){
		throw std::runtime_error("Can't destroy file "+path+" because it doesn't exist");
	}
	ensureFileDoesNotExist(path);
}

void ensureDirectoryDoesNotExist(const std::string& path){
	fs::path dir = path;
	if(fs::exists(dir)){
		// Destroy it >:(
		if(!fs::is_directory(dir)){
			throw std::runtime_error("Can't destroy directory "+path+" because it is a non-directory entity");
		}
		fs::remove_all(dir);
	}

	// Work is done for you
}
	

void destroyDirectory(const std::string& path){
	if(!fs::exists(fs::path(path))){
		throw std::runtime_error("Can't destroy directory "+path+" because it doesn't exist");
	}
	ensureDirectoryDoesNotExist(path);
}

std::string getParentPath(std::string path){
	if(path == ""){
		throw std::runtime_error("Can't get parent path of empty string");
	}
	while(path.back() == '/'){
		if(path != "/"){
			path.pop_back();
		}
	}

	const fs::path fp = path;
	if(!fs::exists(fp)){
		throw std::runtime_error("Can't get parent directory of path that doesn't exist"+path);
	}
	return fp.parent_path().string();
	
}
}
