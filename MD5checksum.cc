/** 
 *	MD5 checksum utility - C++ wrapper around C implementation
 * 
 * @author: parth_shel
 * @version: v:0.1 Apr. 5, 2018
 **/ 

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include <string>
#include <fstream>
#include <iostream>

#include "MD5checksum.hh"

MD5checksum::MD5checksum(std::string file_path) {
	this->file_path = file_path;
	this->generate_hash();
}

MD5checksum::~MD5checksum() {

}

unsigned long MD5checksum::get_file_size(int fd) {
	struct stat statbuf;
	if(fstat(fd, &statbuf) < 0) {
		return 0;
	}
	return statbuf.st_size;
}

void MD5checksum::generate_hash() {
	int fd;
	unsigned char result[MD5_DIGEST_LENGTH];
	void * file_buffer;
	if((fd = open(file_path.c_str(), O_RDONLY)) < 0) {
		return;
	}

	if((file_size = get_file_size(fd)) == 0) {
		return;
	}
	
	file_buffer = mmap(0, file_size, PROT_READ, MAP_SHARED, fd, 0);
	MD5((unsigned char*) file_buffer, file_size, result);
	munmap(file_buffer, file_size);

	close(fd);

	result_hash.clear();
	for(int i = 0;i < MD5_DIGEST_LENGTH;i++) {
		result_hash.push_back(result[i]);
	}
}

void MD5checksum::save_hash() {
	std::string hash_path(file_path);
	hash_path.append(".md5hash");
	std::ofstream hash_file(hash_path, std::ios::out | std::ios::trunc);
	std::string str(result_hash.begin(), result_hash.end());
	hash_file << str; 
	hash_file.close();
}

bool MD5checksum::compare_saved_hash(std::string other_file_path) {
	std::ifstream file(other_file_path, std::ios::in);
	if(!file.good()) {
		return false;
	}

	if(file.is_open()) {
		std::string buf((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		std::string str(result_hash.begin(), result_hash.end());
		return (str == buf);
	}
	return false;
}

/*std::vector MD5checksum::get_hash() {
	return this->result_hash;
}*/

bool MD5checksum::compare_hashes(MD5checksum* that) {
	return (this->result_hash == that->result_hash);	
}

/* For debug purposes */
/*
int main(int argc, char* argv[]) {
	if(argc != 3) {
		std::cout << "please specify file path" << std::endl;
		exit(EXIT_FAILURE);
	}

	MD5checksum* md51 = new MD5checksum(std::string(argv[1]));
	MD5checksum* md52 = new MD5checksum(std::string(argv[2]));

	if(md51->compare_hashes(md52)) {
		std::cout << "files are the same!" << std::endl;
	}
	else {
		std::cout << "files differ.." << std::endl;
	}

	std::string hash_path(argv[2]);
	hash_path.append(".md5hash");
	md52->save_hash();
	if(md51->compare_saved_hash(hash_path)) {
		std::cout << "saved hash matches!" << std::endl;
	}
	else {
		std::cout << "saved hash differ.." << std::endl;
	}

	delete md51;
	delete md52;

	return EXIT_SUCCESS;
}*/
