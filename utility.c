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

char* readFileToStr(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(filesize + 1);
    if (!buffer) {
        perror("Unable to allocate memory");
        fclose(file);
        return 0;
    }

    fread(buffer, 1, filesize, file);
    buffer[filesize] = 0;

    fclose(file);

    return buffer;
}

void writeBytesToFile(unsigned char* bytes, size_t len, char* filename) {
    FILE* file = fopen(filename, "wb");

    if (file == NULL) {
        perror("Error during opening the file");
	    return;
    }

    for (int i=0; i<len; ++i) {
        if (bytes[i] != NULL) {
	        fwrite(&bytes[i], sizeof(unsigned char), 1, file);
	    }
    }

    fclose(file);
}

unsigned char* hexStringToBytes(const char* hexString, size_t* outLength) {
    size_t length = strlen(hexString);
    // Удаляем пробелы
    size_t byteCount = 0;
    for (size_t i = 0; i < length; i++) {
        if (hexString[i] != ' ') {
            byteCount++;
        }
    }
    
    *outLength = byteCount / 2;
    unsigned char* bytes = (unsigned char*)malloc(*outLength);
    if (bytes == NULL) {
        return NULL;
    }

    size_t byteIndex = 0;
    for (size_t i = 0; i < length; i += 3) {
        if (hexString[i] != ' ') {
            sscanf(hexString + i, "%2hhx", &bytes[byteIndex++]);
        }
    }

    return bytes;
}

void fileWrite(char* filename, char* filename_to_write) {
    //printf("Write bytes\n(max=16 for string):\n");

    //fgets(strBytes, sizeof(strBytes), stdin);

    //unsigned char bytes[16];
    char* file_data = readFileToStr(filename);
    printf("file_data: '%s'", file_data);

    size_t out_len;
    unsigned char* bytes = hexStringToBytes(file_data, &out_len);

    //printf("Bytes: '%s'", strBytes);
    
    for (int i=0; i<out_len; i++) {
        printf("%02X\n", bytes[i]);
    }

    writeBytesToFile(bytes, out_len, filename_to_write);
    printf("Success!");
}

int parser_byd(short argc, char* args[]) {
    if (argc <= 1) {
	perror("No argumets given.\nAborting...");
	return 1;
    }

    if (argc == 2) {
	    const char* filename = args[1];
	    size_t size;

	    unsigned char* data = readFileToHexArray(filename, &size);
	    
	    if (data) {
            for (long i=0; i<size; ++i) {
                printf("%02X", data[i]);

                if (i != size-1)
                    printf(" ");
            }

            free(data);
	    }
    } else if (argc == 4) {
	    printf("%d, %s", argc, args[2]);

        if (!strcmp(args[2], "-f")) {
	        fileWrite(args[1], args[3]);
        }
    }

    return 0;
}
