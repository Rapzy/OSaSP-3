#include "stdio.h"
#include "stdlib.h"
#include "dirent.h"
#include "string.h"
#include "sys/stat.h"
#include "limits.h"
#include "libgen.h"
#include "unistd.h"

#define OUT_WORD 0;
#define IN_WORD 1;

void GetDir(char *dir);
void PrintInfo(char *path, int pid);
char *progname;
int num_file=0, num_dir=1;

int  main(int argc, char *argv[]){
	progname = basename(argv[0]);
	GetDir(argv[1]);
	wait(NULL);
	printf("Directories checked: %d\n", num_dir);
	printf("Files checked: %d\n", num_file);
  return 0;
}
void GetDir(char *dir){
	struct dirent *d;
	DIR *dp;
	if((dp = opendir(dir)) == NULL)
    {
      fprintf(stderr, "%s: Couldn't open %s.\n",progname,dir);
      return;
    }
	while ((d = readdir(dp))!= NULL){
		char path[1024];
		char buf[200];
		if (d->d_type == DT_DIR) {
			if (strcmp(d->d_name, ".") == 0 || strcmp(d->d_name, "..") == 0)
	      continue;
	    if(strcmp(dir,"/") == 0){
				snprintf(path, sizeof(path), "/%s", d->d_name);
			}
			else{
				snprintf(path, sizeof(path), "%s/%s", dir, d->d_name);
			}
			num_dir++;
		  GetDir(path);
		}
		else {
			num_file++;
			pid_t pid = fork();
			if(pid == 0){
				snprintf(path, sizeof(path), "%s/%s", dir, d->d_name);
				FILE *file = fopen(path, "r");
				char symbol;
				int pos = IN_WORD;
				int words_count = 0;
				while ((symbol = fgetc(file)) != EOF){
					if(symbol == ' ' || symbol == '\n' || symbol == '\t'){
						if(pos == 1){ 
							words_count++;
							pos = OUT_WORD;
						}
					}
					else if (pos == 0)
						pos = IN_WORD;
				}
				PrintInfo(path, getpid());
				exit(0);
			}
			else if (pid > 0)
				printf ("Parent process (pid=%d)\n", getpid());
		}
	}
	closedir(dp);
}

void PrintInfo(char *path, int pid){
	struct stat fileStat;
	char buf[PATH_MAX];
	stat(path, &fileStat);
	printf("%d %s %lld\n",pid, realpath(path,buf),fileStat.st_size);
}