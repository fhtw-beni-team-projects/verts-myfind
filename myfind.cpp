#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/types.h>
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
	/* option parsing */

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



	/* arguments */

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



	/* forks */

	pid_t pid;

	for ( auto& filename : filenames ) {
		pid = fork();

		switch (pid) {
			case -1:
				/* error handling */
				continue;
			case 0:
				search(searchpath, filename, case_sensitive, recursive);
				break;
			default:
				std::cout << "Created child with PID " << pid << std::endl;
				continue;
		}

		break;
	}



   	/* Copy-pasted from the example repo */
   	/* irrelevant with filesystem library */

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

		// TODO: if-statement limiting to matching files
		std::cout << getpid() << ": " << entry.path().filename() << ": " << fs::canonical(fs::absolute(entry.path())) << std::endl;
	}
}