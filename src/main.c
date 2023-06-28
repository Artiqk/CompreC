#include "zip_tools.h"

void printHelp();

int main(int argc, char *argv[]) {

	printHelp();

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