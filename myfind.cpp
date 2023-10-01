#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void search(int pipe[2], std::string searchpath, std::string filename, bool case_sensitive, bool recursive);

int main(int argc, char* argv[])
{
	/* option parsing */

	bool case_sensitive = true;
	bool recursive = false;

	int c;
	while ((c = getopt(argc, argv, "iR")) != EOF) {
		switch (c) {
			case '?':
				std::cerr << "unkown option";
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


	/* pipe */

	int fd[2];
    if (pipe(fd) != 0) {
        std::cerr << "error creating pipe";
        return EXIT_FAILURE;
    }

	/* forks */

	pid_t pid;

	for ( auto& filename : filenames ) {
		pid = fork();

		switch (pid) {
			case -1:
				std::cerr << "error creating child process for filename " + filename;
				continue;
			case 0:
				search(fd, searchpath, filename, case_sensitive, recursive);
				close(fd[1]);
				return 1;
			default:
//				std::cout << "Created child with PID " << pid << std::endl;
				continue;
		}

		break;
	}

	/* synchronized output using pipes */

    char buffer[PIPE_BUF];
    memset(buffer, 0, sizeof(buffer));
	
	close(fd[1]);

    while (read(fd[0], buffer, PIPE_BUF) != 0) {
        std::cout << buffer << std::endl;
        memset(buffer, 0, sizeof(buffer));
    }

	/* zombie slayer 9000 */

	pid_t childpid;

	while ((childpid = waitpid(-1, NULL, WNOHANG))) {
		if ((childpid == -1) && (errno != EINTR)) {
			break;
		}
	}
}


void search(int pipe[2], std::string searchpath, std::string filename, bool case_sensitive, bool recursive)
{
	// close read
	close(pipe[0]);

	/* using C filehandling */

	struct dirent* de;
	DIR* dr;
	if ((dr = opendir(searchpath.c_str())) == NULL) {
    	std::cerr << "failed to open directory";
    	return;
	}

	// set compare function (case sensitive/insensitve)
	int (*compptr)(const char *, const char *) = case_sensitive ? strcmp : strcasecmp;	

	while ((de = readdir(dr)) != NULL) {
		if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, "..")) {
			continue;
		}

		if (de->d_type == DT_DIR) {
			if (recursive)
				search(pipe, searchpath + "/" + de->d_name, filename, case_sensitive, recursive);
			continue;
		}

		if ((*compptr)(filename.c_str(), de->d_name)) {
			continue;
		}

		char resolved_path[pathconf(".", _PC_PATH_MAX)];
		realpath((searchpath + "/" + de->d_name).c_str(), resolved_path);
		//std::cout << getpid() << ": " << de->d_name << ": " << resolved_path << std::endl;
		std::string output = std::to_string(getpid()) + ": " + de->d_name + ": " + resolved_path;
		write(pipe[1], output.c_str(), output.length());
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