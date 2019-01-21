#include "readimg.h"
#include <iostream>
void getFileName(const char* folder, vector<string> &files){
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folder)) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if(ent->d_type == DT_REG){
                files.push_back(ent->d_name);
            }
        }
        closedir(dir);
    } else {
        /* could not open directory */
        printf("Cannot open directory %s\n", folder);
        exit(EXIT_FAILURE);
    }
}