#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void search(std::string searchpath, std::string filename, bool case_sensitive, bool recursive);

int main(int argc, char* argv[])
{
	/* option parsing */

	bool case_sensitive = true;
	bool recursive = false;

	while (true) {
		switch (getopt(argc, argv, "iR")) {
			case '?':
				perror("unkown option");
			case 'i':
				case_sensitive = false;
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

//	std::cout << "searchpath: " << searchpath << std::endl;
//	std::cout << "filename(s): ";
//	for ( auto& filename : filenames) {
//		std::cout << filename << ", ";
//	}
//	std::cout << std::endl;



	/* forks */

	pid_t pid;

	for ( auto& filename : filenames ) {
		pid = fork();

		switch (pid) {
			case -1:
				perror("error creating child process");
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

	// TODO: handle child-process termination
}


void search(std::string searchpath, std::string filename, bool case_sensitive, bool recursive)
{
	/* using C filehandling */

	struct dirent* de;
	DIR* dr;
	if ((dr = opendir(searchpath.c_str())) == NULL) {
    	perror("failed to open directory");
    	return;
	}

	// set compare function (case sensitive/insensitve)
	int (*compptr)(const char *, const char *) = case_sensitive ? strcmp : strcasecmp;	

	while ((de = readdir(dr)) != NULL) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
			continue;
		}

		if (recursive && de->d_type == DT_DIR) {
			search(searchpath + "/" + de->d_name, filename, case_sensitive, recursive);
			continue;
		}

		if (!(*compptr)(filename.c_str(), de->d_name)) {
			char* resolved_path = (char*)malloc(pathconf(".", _PC_PATH_MAX));
			realpath((searchpath + "/" + de->d_name).c_str(), resolved_path);
			std::cout << getpid() << ": " << de->d_name << ": " << resolved_path << std::endl;
			free(resolved_path);
		}
	}

	while ((closedir(dr) == -1) && (errno == EINTR));

	/* using C++ filesystem library */

	/*for ( auto& entry : fs::directory_iterator(searchpath)) { // can we use recursive_directory_iterator()?
		if (recursive && entry.is_directory()) {
			search(entry.path(), filename, case_sensitive, recursive);
			continue;
		}

		int (*compptr)(const char *, const char *) = case_sensitive ? strcmp : strcasecmp;

		if (!(*compptr)(filename.c_str(), entry.path().filename().u8string().c_str())) {
			std::cout << getpid() << ": " << entry.path().filename().u8string() << ": " << fs::canonical(fs::absolute(entry.path())).u8string() << std::endl;
		}
	}*/
}