#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <cstring>

int main(int argc, char* argv[]) {
	std::cout << "Hello World!" << std::endl; // This code prints "Hello World!" to the console

	while (true) {
		switch (getopt(argc, argv, "iR")) {
			case 'i':
				std::cout << "Specified -i" << std::endl;
				continue;
			case 'R':
				std::cout << "Specified -R" << std::endl;
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


	struct dirent *direntp;
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
	std::cout << std::endl;
}