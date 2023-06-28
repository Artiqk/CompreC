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


void displayFileList(zip_t* archive, FileInfo* fileInfoList) {

	zip_int64_t totalFiles = zip_get_num_entries(archive, 0);

	int numberOfFiles = 0, totalSize = 0;

	printf("     Size       Encrypted  Name\n");
	printf("--------------  ---------  -----------\n");

	for (int fileIndex = 0; fileIndex < totalFiles; fileIndex++) {
		char* fileName = fileInfoList[fileIndex].fileName;

		size_t fileNameLength = fileInfoList[fileIndex].length;

		int fileSize = fileInfoList[fileIndex].size;

		totalSize += fileSize;

		char lastCharacter = fileName[fileNameLength - 1];

		int isDirectory = (lastCharacter == '/');

		if (isDirectory) {
			printf("%14d  %9s  %s (directory)\n", fileIndex, "-", fileName);
			continue;
		}

		numberOfFiles++;

		char* encrypted = isZipFileEncrypted(archive, fileName) ? "yes" : "no";

		printf("%14d  %9s  %s\n", fileSize, encrypted, fileName);

	}

	printf("--------------             -----------\n");
	printf("%14d             %4d files\n", totalSize, numberOfFiles);

}


int extractFileFromZip(zip_t* archive, zip_file_t* zipFile, const char* filePath, const char* destinationPath) {
    // Get information about the file
    zip_stat_t sb;

    if (zip_stat(archive, filePath, 0, &sb) != 0) {
        fprintf(stderr, "[-] Failed to get information about file %s (%s)\n", filePath, __func__);
        zip_fclose(zipFile);
        return -1;
    }

    // Total size of the file
    zip_uint64_t totalSize = sb.size;

    // Open file to write extracted data
    FILE* file = fopen(destinationPath, "wb");

    if (file == NULL) {
        fprintf(stderr, "[-] Failed to open file %s (%s)\n", destinationPath, __func__);
        zip_fclose(zipFile);
        return -1;
    }

    int bufferSize = 4096, bytesRead, totalBytesRead = 0;
    char* buffer = malloc(bufferSize);

    // Read data from zip file and write it to the file
    while ((bytesRead = zip_fread(zipFile, buffer, bufferSize)) > 0) {
        fwrite(buffer, bytesRead, 1, file);
        totalBytesRead += bytesRead;
        printProgressBar(totalBytesRead, totalSize);
    }

    printf("\n");

    free(buffer);

    fclose(file);
    
    zip_fclose(zipFile);

    return 0;
}


int openZipFile(zip_t* archive, const char* filePath, const char* destinationPath) {

    zip_file_t* zipFile = zip_fopen(archive, filePath, 0);

    if (zipFile == NULL) {
        fprintf(stderr, "[-] Failed to open file %s (%s)\n", filePath, __func__);
        return 1;
    }

    return extractFileFromZip(archive, zipFile, filePath, destinationPath);
}


int openZipFileEncrypted(zip_t* archive, const char* filePath, const char* destinationPath, const char* password) {

    zip_file_t* zipFile = zip_fopen_encrypted(archive, filePath, 0, password);

    if (zipFile == NULL) {
        fprintf(stderr, "[-] Failed to open encrypted file %s (%s)\n", filePath, __func__);
        return 1;
    }

    return extractFileFromZip(archive, zipFile, filePath, destinationPath);
}


int addFileToZip(zip_t* archive, const char* fileName, const char* filePathInZip, const char* password) {

    // Validate inputs
    if (!archive || !fileName || !filePathInZip) {
        fprintf(stderr, "[-] Invalid arguments\n");
        return -1;
    }

    // Create zip source from file
    zip_source_t* zipSource = zip_source_file(archive, fileName, 0, -1);

    if (!zipSource) {
        fprintf(stderr, "[-] Failed to create zip source from file %s (%s)\n", fileName, __func__);
        return -1;
    }

    // Add file to zip archive
    zip_int64_t fileIndex = zip_file_add(archive, filePathInZip, zipSource, ZIP_FL_OVERWRITE);

    if (fileIndex < 0) {
        fprintf(stderr, "[-] Failed to add file %s to zip archive (%s)\n", fileName, __func__);
        zip_source_free(zipSource);
        return -1;
    }

    if (password && zip_file_set_encryption(archive, fileIndex, ZIP_EM_AES_256, password) < 0) {
        fprintf(stderr, "[-] Failed to set password for file %s (%s)\n", fileName, __func__);
        zip_source_free(zipSource);
        return -1;
    }

    return 0;
}


int insertFileToZip(zip_t* archive, const char* fileName, const char* filePathInZip) {
    int error = addFileToZip(archive, fileName, filePathInZip, NULL);

    if (error) {
        return 1;
    }

    return 0;
}


int insertEncryptedFileToZip(zip_t* archive, const char* fileName, const char* filePathInZip, const char* password) {
    int error = addFileToZip(archive, fileName, filePathInZip, password);

    if (error) {
        return 1;
    }

    return 0;
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


int tryPasswordOnZipFile(zip_t* archive, const char* filePath, const char* password) { // FIXME - Rewrite this function to handle the password verification with the checksum
    zip_file_t* zipFile = zip_fopen_encrypted(archive, filePath, (zip_flags_t)0, password);

    if (zipFile == NULL) {
        return 1;
    }

    char buffer[100];
    memset(buffer, 0, sizeof(buffer));

    zip_int64_t readBytes = zip_fread(zipFile, &buffer, sizeof(buffer) - 1);

    if (readBytes < 0) {
        int archive_errno = zip_error_code_zip(zip_get_error(archive));

        if (archive_errno == ZIP_ER_WRONGPASSWD) {
            zip_fclose(zipFile);
            return 1;
        }
    }

    for(int i = 0; i < readBytes; i++) {
        // If character is not a printable ASCII or a whitespace character
        if (!isprint((unsigned char)buffer[i]) && !isspace((unsigned char)buffer[i])) {
            zip_fclose(zipFile);
            return 1; // Likely the wrong password
        }
    }
    
    zip_fclose(zipFile);
    return 0;
}


int getTotalPasswordsInFile(FILE* file) {
	int totalPasswords = 0;
	char ch;

	while (!feof(file)) {
		ch = fgetc(file);
		if (ch == '\n') {
			totalPasswords++;
		}
	}

	rewind(file);

	return totalPasswords;
}


char* bruteforceZipWithDictionary(zip_t* archive, const char* filePath, const char* dictionaryPath) {
	FILE* file = fopen(dictionaryPath, "r");

	if (file == NULL) {
		printf("Failed to open file %s (%s)", dictionaryPath, __func__);
		exit(1);
	}

	int totalPasswords = getTotalPasswordsInFile(file);

	char* password = NULL;
	size_t len = 0;
	int attemptedPasswords = 0;

	while (getline(&password, &len, file) != -1) {
		password[strcspn(password, "\n")] = 0; // Remove newline character

		if (tryPasswordOnZipFile(archive, filePath, password) == 0) {
			printProgressBar(attemptedPasswords, totalPasswords);
			fclose(file);
			return password;
		}

		attemptedPasswords++;
		printProgressBar(attemptedPasswords, totalPasswords);
	}

	free(password);
	
	fclose(file);

	return NULL;
}