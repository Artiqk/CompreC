#ifndef ZIP_TOOLS_H
#define ZIP_TOOLS_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>  // for isprint() and isspace()
#include <getopt.h>
#include <zip.h>

typedef struct {
	char* fileName;
	size_t length;
	int size;
} FileInfo;

int isZipFileEncrypted(zip_t* archive, const char* fileName);

int getFileSize(zip_t* archive, zip_uint64_t index);

FileInfo* getZipFilesInfoList(zip_t *archive);

void freeFileInfoList(zip_t* archive, FileInfo* fileInfoList);

int extractFileFromZip(zip_t* archive, zip_file_t* zipFile, const char* filePath, const char* destinationPath);

void displayFileList(zip_t* archive, FileInfo* fileInfoList);

int openZipFile(zip_t* archive, const char* filePath, const char* destinationPath);

int openZipFileEncrypted(zip_t* archive, const char* filePath, const char* destinationPath, const char* password);

int addFileToZip(zip_t* archive, const char* fileName, const char* filePathInZip, const char* password);

int insertFileToZip(zip_t* archive, const char* fileName, const char* filePathInZip);

int insertEncryptedFileToZip(zip_t* archive, const char* fileName, const char* filePathInZip, const char* password);

void printProgressBar(int current, int total);

int tryPasswordOnZipFile(zip_t* archive, const char* filePath, const char* password);

int getTotalPasswordsInFile(FILE* file);

char* bruteforceZipWithDictionary(zip_t* archive, const char* zipFile, const char* dictionaryPath);

#endif // ZIP_TOOLS_H
