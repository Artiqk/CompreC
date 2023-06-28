#include "zip_tools.h"

void printHelp();

zip_t* openArchive(const char* zipPath);

void listFiles(zip_t* archive);

void bruteforcePassword(zip_t* archive, const char* dictionaryFile, const char* fileToBruteforce);

int main(int argc, char *argv[]) {

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"zip", required_argument, 0, 'z'},
		{"target_file", required_argument, 0, 't'},
		{"output", required_argument, 0, 'o'},
		{"list", no_argument, 0, 'l'},
		{"dictionary", required_argument, 0, 'd'},
		{"password", required_argument, 0, 'p'},
		{"extract", no_argument, 0, 'e'},
		{"include", no_argument, 0, 'i'},
		{0, 0, 0, 0}
	};

	int opt = 0, extract = 0, include = 0;

	zip_t* archive;

	char* zipPath = NULL;
	char* password = NULL;
	char* targetFile = NULL;
	char* outputFile = NULL;
	char* dictionaryFile = NULL;

    while ((opt = getopt_long(argc, argv, "hz:t:o:ld:p:ei", long_options, NULL)) != -1) {

		switch (opts) {
			case 'h':
				printHelp();
				exit(EXIT_SUCCESS);
			case 'z':
				zipPath = optarg;
				archive = openArchive(zipPath);
				break;
			case 't':
				targetFile = optarg;
				break;
			case 'o':
				outputFile = optarg;
				break;
			case 'l':
				listFiles(archive);
				break;
			case 'd':
				dictionaryFile = optarg;
				bruteforcePassword(archive, dictionaryFile, targetFile);
				break;
			case 'p':
				password = optarg;
				break;
			case 'e':
				extract = 1;
				break;
			case 'i':
				if (!extract) {
					include = 1;
				}
				break;
			default:
				printHelp();
				break;
		}

    }

	// Check if path and destination files are not missing
	if (extract || include) { 
		if (targetFile == NULL || outputFile == NULL) {
			fprintf(stderr, "[-] Missing arguments\n");
			exit(EXIT_FAILURE);
		}
	}

	// Extract file from zip
	if (extract) {
		if (password != NULL) {
			openZipFileEncrypted(archive, targetFile, outputFile, password);
		} else {
			openZipFile(archive, targetFile, outputFile);
		}
	}

	// Add file to zip
	if (include) {
		if (password != NULL) {
			insertEncryptedFileToZip(archive, targetFile, outputFile, password);
		} else {
			insertFileToZip(archive, targetFile, outputFile);
		}
	}

	// Close zip file
	if (zip_close(archive) < 0) {
		printf("Failed to close zip file\n");
		return EXIT_FAILURE;
	}

    return EXIT_SUCCESS;
}


void printHelp() {
    printf("Usage: ./zip [OPTIONS]...\n");
    printf("Manipulate a zip file, such as extracting a file, or including a file.\n");
    printf("\nOptions:\n");
    printf("  -h, --help                        Show this help message and exit\n");
    printf("  -z, --zip ZIPFILE                 Specify the path of the zip file to operate on\n");
    printf("  -t, --target_file TARGETFILE      Specify the target file to be extracted from the zip file or to be included into the zip file\n");
    printf("  -o, --output OUTPUTFILE           Specify the output file name\n");
    printf("  -l, --list                        List all files in the zip\n");
    printf("  -d, --dictionary DICTFILE         Specify the path of the dictionary file for password brute-forcing\n");
    printf("  -p, --password PASSWORD           Specify a password to use for extraction or inclusion\n");
    printf("  -e, --extract                     Extract the target file from the zip file\n");
    printf("  -i, --include                     Include the target file into the zip file\n");
}



zip_t* openArchive(const char* zipPath) {
	if (zipPath == NULL) {
		fprintf(stderr, "[-] No path for zip file provided\n");
		exit(EXIT_FAILURE);
	}

	zip_t* archive = zip_open(zipPath, ZIP_CREATE | ZIP_CHECKCONS, NULL);

	if (archive == NULL) {
		fprintf(stderr, "[-] Error while trying to open archive %s\n", zipPath);
		exit(EXIT_FAILURE);
	}

	return archive;
}


void listFiles(zip_t* archive) {
	FileInfo* fileInfoList = getZipFilesInfoList(archive);
	displayFileList(archive, fileInfoList);
	freeFileInfoList(archive, fileInfoList);

	if (zip_close(archive) < 0) {
		printf("Failed to close zip file\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}


void bruteforcePassword(zip_t* archive, const char* dictionaryFile, const char* fileToBruteforce) {

	if (fileToBruteforce == NULL) {
		fprintf(stderr, "[-] Missing file options\n");
		exit(EXIT_FAILURE);
	}

	char* password = bruteforceZipWithDictionary(archive, fileToBruteforce, dictionaryFile);

	if (password != NULL) {
		printf("\n[+] PASSWORD FOUND : %s\n", password);
	} else {
		printf("\n[-] PASSWORD NOT FOUND\n");
	}

	if (zip_close(archive) < 0) {
		printf("Failed to close zip file\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}