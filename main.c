#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <locale.h>

#define MAXPATH 4096

enum conditions {
	NONE,
    LIST,
	ALL,
	LIST_ALL  = ALL + LIST
};

void create_mode_string(mode_t mode, char *str) {
	if (S_ISREG(mode))       str[0] = '-';
	else if (S_ISDIR(mode))  str[0] = 'd';
	else if (S_ISCHR(mode))  str[0] = 'c';
	else if (S_ISBLK(mode))  str[0] = 'b';
	else if (S_ISFIFO(mode)) str[0] = 'p';
	else if (S_ISLNK(mode))  str[0] = 'l';
	else if (S_ISSOCK(mode)) str[0] = 's';
	else                     str[0] = '?';

	str[1] = (mode & S_IRUSR) ? 'r' : '-';
	str[2] = (mode & S_IWUSR) ? 'w' : '-';
	str[3] = (mode & S_IXUSR) ? 'x' : '-';

	str[4] = (mode & S_IRGRP) ? 'r' : '-';
	str[5] = (mode & S_IWGRP) ? 'w' : '-';
	str[6] = (mode & S_IXGRP) ? 'x' : '-';

	str[7] = (mode & S_IROTH) ? 'r' : '-';
	str[8] = (mode & S_IWOTH) ? 'w' : '-';
	str[9] = (mode & S_IXOTH) ? 'x' : '-';

	str[10] = '\0';
}

void print_normal(int show_all, size_t path_s, char full_path[], char *delim)
{
	struct dirent **namelist;
	int i,n;
	struct stat sb;
	size_t d_name_s;

	n = scandir(full_path, &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		for (i = 0; i < n; i++) {

			d_name_s = strlen(namelist[i]->d_name);

			if (path_s + d_name_s > MAXPATH) {
				fprintf(stderr, "File Name too big\n");
				exit(EXIT_FAILURE);
			}

			strncat(full_path, namelist[i]->d_name, d_name_s);
			full_path[path_s + d_name_s] = '\0';

			if (lstat(full_path, &sb) == -1) {
				printf("%s\n", full_path);
				perror("lstat");
				exit(EXIT_FAILURE);
			}

			full_path[path_s] = '\0';

			if (show_all == 0 && namelist[i]->d_name[0] == '.')
				continue;
			else
				printf("%s%s", namelist[i]->d_name, delim);
		}
	}
}

void print_long(size_t path_s, char full_path[])
{
	struct dirent **namelist;
	int i,n;
	struct passwd  *pw;
	struct group   *gr;
	struct stat     sb;
	struct tm      *tm;
	char   datestring[256];
	char   mode_string[11];
	size_t d_name_s;

	n = scandir(full_path, &namelist, 0, alphasort);
	if (n < 0)
		perror("scandir");
	else {
		for (i = 0; i < n; i++) {

			d_name_s = strlen(namelist[i]->d_name);

			if (path_s + d_name_s > MAXPATH) {
				fprintf(stderr, "File Name too big\n");
				exit(EXIT_FAILURE);
			}

			strncat(full_path, namelist[i]->d_name, d_name_s);
			full_path[path_s + d_name_s + 1] = '\0';

			if (lstat(full_path, &sb) == -1) {
				printf("%s\n", full_path);
				perror("lstat");
				exit(EXIT_FAILURE);
			}
			tm = localtime(&sb.st_mtime);

			/* Get localized date string. */
			strftime(datestring, sizeof(datestring), "%b %e %H:%M", tm);

			pw = getpwuid(sb.st_uid);
			gr = getgrgid(sb.st_gid);

			create_mode_string(sb.st_mode, mode_string);

			printf("%s %2ld %s %s %5ld %s %s\n",
					mode_string,
					sb.st_nlink,
					pw->pw_name,
					gr->gr_name,
					(long)sb.st_size,
					datestring,
					namelist[i]->d_name
				  );

			full_path[path_s] = '\0';
		}
	}
}

int main(int argc, char *argv[])
{
	int flags, opt;
	flags = 0;
	while ((opt = getopt(argc, argv, "la")) != -1) {
		switch (opt) {
			case 'l': flags |= 1; break;
			case 'a': flags |= 2; break;
			default: /* '?' */
					  fprintf(stderr, "Usage: %s [-l] [-a] directory\n", argv[0]);
					  exit(EXIT_FAILURE);
		}
	}

	if (argc == optind) {
		argv[argc++] = ".";
	}

	DIR    *dirp;
	char   *path;
	char   full_path[MAXPATH] = "";
	size_t path_s;


	for (int i = optind; i < argc; i++) {

		path = argv[i];
		dirp = opendir(path);

		if (dirp == NULL) {
			perror("opendir");
			continue;
		}

		path_s = strlen(argv[i]);

		if (path_s < MAXPATH) {

			strncpy(full_path, path, path_s);

			if (full_path[path_s] != '/') {
				full_path[path_s] = '/';
				path_s++;
			}

			full_path[path_s] = '\0';

		} else {
			fprintf(stderr, "Directory Name too big\n");
			exit(EXIT_FAILURE);
		}

		switch (flags) {
			case LIST:
				print_normal(0, path_s, full_path, "\n");
				break;
			case ALL:
				print_normal(1, path_s, full_path, "  ");
				printf("\n");
				break;
			case LIST_ALL:
				print_long(path_s, full_path);
				break;
			default:
				print_normal(0, path_s, full_path, "  ");
				printf("\n");
				break;
		}
	}
	closedir(dirp);

	return 0;
}
