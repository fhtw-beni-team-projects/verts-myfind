#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

void search(std::string searchpath, std::string filename, bool case_sensitive, bool recursive);

int main(int argc, char* argv[])
{
	bool case_sensitive = false;
	bool recursive = false;

	while (true) {
		switch (getopt(argc, argv, "iR")) {
			case 'i':
				case_sensitive = true;
				continue;
			case 'R':
				recursive = true;
				continue;
			case -1:
				break;
		}

		break;
	}

	std::string searchpath;
	std::vector<std::string> filenames;

	if (optind < argc) {
		searchpath = argv[optind];
	}
	optind++;
	for (; optind < argc; optind++) {
		filenames.push_back(argv[optind]);
	}

	/* Temporary print statements */
	std::cout << "searchpath: " << searchpath << std::endl;
	std::cout << "filename(s): ";
	for ( auto& filename : filenames) {
		std::cout << filename << ", ";
	}
	std::cout << std::endl;


	search(searchpath, filenames[0], case_sensitive, recursive);


   	/* Copy-pasted from the example repo */

	/*struct dirent *direntp;
	DIR *dirp;

	if ((dirp = opendir(searchpath.c_str())) == NULL) {
    	perror("Failed to open directory");
    	return 1;
   	}

	std::cout << "files: ";
	while ((direntp = readdir(dirp)) != NULL)
    	printf("%s\n", direntp->d_name);
	while ((closedir(dirp) == -1) && (errno == EINTR))
    	; 
	std::cout << std::endl;*/
}

void search(std::string searchpath, std::string filename, bool case_sensitive, bool recursive)
{
	for ( auto& entry : fs::directory_iterator(searchpath)) { // can we use recursive_directory_iterator()?
		if (recursive && entry.is_directory()) {
			search(entry.path(), filename, case_sensitive, recursive);
			continue;
		}

		std::cout << entry.path().filename() << std::endl;
	}
}