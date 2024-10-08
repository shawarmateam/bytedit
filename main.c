#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned char* readFileToHexArray(const char* filename, size_t* outSize) {
    FILE* file = fopen(filename, "rb"); // Open file on read in bin mode
    
    if (!file) {
        perror("Can't open file.\nAborting...");

	return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* buffer = (unsigned char*) malloc(fileSize);

    if (!buffer) {
        perror("Can't allocate memory.\nAborting...");
	fclose(file);

	return NULL;
    }

    // Reading the file
    size_t bytesRead = fread(buffer, 1, fileSize, file);
    if (bytesRead != fileSize) {
       perror("Error during reading file.\nAborting...");
       free(buffer);
       fclose(file);

       return NULL;
    }

    fclose(file);
    *outSize = fileSize;

    return buffer;
}


void writeBytesToFile(unsigned char bytes[16], char* filename) {
    FILE* file = fopen(filename, "wb");

    if (file == NULL) {
        perror("Error during opening the file");
	return;
    }

    for (int i=0; i<16; ++i) {
        if (bytes[i] != NULL) {
	    fwrite(&bytes[i], sizeof(unsigned char), 1, file);
	}
    }

    fclose(file);
}

int main(int argc, char* args[]) {
    if (argc <= 1) {
	perror("No argumets given.\nAborting...");
	return 1;
    }

    if (argc == 2) {
	    const char* filename = args[1];
	    size_t size;

	    unsigned char* data = readFileToHexArray(filename, &size);
	    
	    if (data) {
		printf("File size: '%zu' bytes\nData:\n", size);

		for (long i=0; i<size; ++i) {
		    printf("%02X ", data[i]);
		}
		printf("\n");

		free(data);
	    }
    } else if (argc == 3) {
	printf("%d, %s", argc, args[2]);
        if (strcmp(args[2], "-f") == 0) {
	    int maxStrLength = 16*3; // 48
	    char strBytes[maxStrLength];
	    printf("Write bytes\n(max=16 for string):\n");

            fgets(strBytes, sizeof(strBytes), stdin);

	    unsigned char bytes[16];

	    int i;
	    for (i=0; i<16; ++i) {
	        sscanf(strBytes+i*3, "%2hhX", &bytes[i]);
	    }

	    for (i=0; i<16; ++i) {
	        printf("bytes[%d] = 0x%02X\n", i, bytes[i]);
	    }

	    printf("Success!");
	    printf("Bytes: '%s'", strBytes);
	    writeBytesToFile(bytes, args[1]);
	}
    }

    return 0;
}
