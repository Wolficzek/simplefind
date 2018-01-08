#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>

int find(char * path, char * argv[]){
     /* stat - get file status */
    struct stat mystat;
    struct dirent *dptr = NULL;
    struct group *grp = NULL;
    struct passwd *pwd = NULL;
    char cwd[1024];
    DIR *dp;
    dp = opendir((const char*)path);
        while((dptr = readdir(dp)) != NULL){
            stat(dptr->d_name, &mystat);

            if( (strcmp(dptr->d_name, argv[1])) == 0){
                /*Dane o pliku*/
                pwd = getpwuid(mystat.st_uid);
                grp = getgrgid(mystat.st_gid);
                printf("Name: %s\n", dptr->d_name);
                printf((S_ISDIR(mystat.st_mode)) ? "directory\n" : "");
                printf((S_ISREG(mystat.st_mode)) ? "file\n" : "");
                printf("Pwd: %s\n", pwd->pw_name);
                printf("Grp: %s\n", grp->gr_name);
                /*Uprawnienia*/
                printf((mystat.st_mode & S_IRUSR) ? "r" : "-");
                printf((mystat.st_mode & S_IWUSR) ? "w" : "-");
                printf((mystat.st_mode & S_IXUSR) ? "x" : "-");
                printf((mystat.st_mode & S_IRGRP) ? "r" : "-");
                printf((mystat.st_mode & S_IWGRP) ? "w" : "-");
                printf((mystat.st_mode & S_IXGRP) ? "x" : "-");
                printf((mystat.st_mode & S_IROTH) ? "r" : "-");
                printf((mystat.st_mode & S_IWOTH) ? "w" : "-");
                printf((mystat.st_mode & S_IXOTH) ? "x\n" : "-\n");

                if(S_ISREG(mystat.st_mode)) printf("%ld \n", mystat.st_size);
                closedir(dp);
                exit(1);
            }

            if((strcmp(dptr->d_name, ".") != 0) && (strcmp(dptr->d_name, "..") != 0)){
                if((S_ISDIR(mystat.st_mode))){
                    /* Przeskakujemy po sciezkach katalogu za pomoca chdir
                     * bo stat nie ogarnia i szuka zawsze w tej sciezce
                     * gdzie odpala sie program
                     */
                    chdir(dptr->d_name);
                    getcwd(cwd, 1024);
                    find(cwd, argv);
                    chdir("..");
                    getcwd(cwd, 1024);
                }
            }
        }
    return 0;
}

int main(int argc, char * argv[]) {
    char * current_directory = NULL;
    DIR *dp = NULL;
    char firstcwd[1024];
    
    if(argc >= 2){
        /* obsluga bledow */
        if(argc > 3){
            printf("Invalid arguments. [file to find] [starting directory]\n");
            return -1;
        }

        current_directory = getenv("PWD");
        if(current_directory == NULL) {
            printf("Cant open directory.\n");
            return -1;
        }
        /*Szukaj w katalogu*/
        if(argc == 3){
            dp = opendir(argv[2]);
            if(dp == NULL){
                printf("Cant open directory(errno): %d - invalid directory or directory doesnt exists\n", errno);
                return -1;
            }
            chdir(argv[2]);
            getcwd(firstcwd, 1024);
            find(firstcwd, argv);
        
        }else{
            find(current_directory, argv);
        }
    }
    printf("Cant find file :(\n");
    return 0;
}

