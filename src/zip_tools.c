#include "zip_tools.h"

int extractFileFromZip(zip_file_t* zipFile, const char* filePath, const char* destinationPath) {
	
	FILE* file = fopen(destinationPath, "wb");

	if (file == NULL) {
		fprintf(stderr, "[-] Failed to open file %s (%s)\n", destinationPath, __func__);
		zip_fclose(zipFile);
		return -1;
	}

	int bufferSize = 4096;
	char* buffer = malloc(bufferSize);
	int bytesRead;

	while ((bytesRead = zip_fread(zipFile, buffer, bufferSize)) > 0) {
		fwrite(buffer, bytesRead, 1, file);
	}

	printf("[+] Extracted file %s\n", filePath);

	free(buffer);
	fclose(file);
	zip_fclose(zipFile);

	return 0;
}
