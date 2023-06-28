#ifndef ZIP_TOOLS_H
#define ZIP_TOOLS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>  // for isprint() and isspace()
#include <getopt.h>
#include <zip.h>

int extractFileFromZip(zip_file_t* zipFile, const char* filePath, const char* destinationPath);

void printProgressBar(int current, int total);

#endif // ZIP_TOOLS_H
