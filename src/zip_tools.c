#include "zip_tools.h"

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
