#include "zip_tools.h"

int isZipFileEncrypted(zip_t* archive, const char* fileName) {
	zip_stat_t zipFileStats;

	// Get stats of file
	int fetchStatsResult = zip_stat(archive, fileName, 0, &zipFileStats);

	// Handle error for getting stats
	if (fetchStatsResult == 0) {
		// Check if file is encrypted
		int isFileEncrypted = (zipFileStats.encryption_method != ZIP_EM_NONE);

		if (isFileEncrypted) {
			return 1;
		}
	}
	else {
		// Failed to get stats
		return -1;
	}

	// File is not encrypted
	return 0;
}


int getFileSize(zip_t* archive, zip_uint64_t index) {
	struct zip_stat st;
	
	if (zip_stat_index(archive, index, 0, &st) != 0) {
		fprintf(stderr, "[-] Failed to get file info for entry %ld (%s)\n", index, __func__);
		return 0;
	}

	return st.size;
}


FileInfo* getZipFilesInfoList(zip_t *archive) {
	zip_int64_t totalFiles = zip_get_num_entries(archive, 0);

	// Handle error for getting total files
	if (totalFiles < 0) {
		fprintf(stderr, "[-] Failed to get total files (%s)\n", __func__);
		return NULL;
	}

	// Allocate memory for file list
	FileInfo* fileInfoList = malloc(sizeof(FileInfo) * totalFiles);

	for (zip_uint64_t fileIndex = 0; fileIndex < totalFiles; fileIndex++) {
		const char* name = zip_get_name(archive, fileIndex, 0);

		// If error, skip to next file
		if (name == NULL) {
			fprintf(stderr, "[-] Failed to get name of entry %d (%s)\n", (int)fileIndex, __func__);
			continue;
		}

		size_t fileNameLength = strlen(name);

		fileInfoList[fileIndex].fileName = malloc(fileNameLength + 1);
		
		strcpy(fileInfoList[fileIndex].fileName, name);

		fileInfoList[fileIndex].length = fileNameLength;

		fileInfoList[fileIndex].size = getFileSize(archive, fileIndex);
		
	}

	return fileInfoList;
}


void freeFileInfoList(zip_t* archive, FileInfo* fileInfoList) {
	zip_int64_t totalFiles = zip_get_num_entries(archive, 0);

	for (int fileIndex = 0; fileIndex < totalFiles; fileIndex++) {
		// Free memory for all file names
		free(fileInfoList[fileIndex].fileName);
	}

	// Free memory for file list struct
	free(fileInfoList);
}


void printProgressBar(int current, int total) {
    const int barWidth = 70;

    printf("\r[");
    int pos = (int)((float)barWidth * current / total);
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) printf("=");
        else if (i == pos) printf(">");
        else printf(" ");
    }
    printf("] %d%%", (int)((float)current * 100.0 / total));
    fflush(stdout);  // Force the line to print
}