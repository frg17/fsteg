#include "stdio.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

struct Pixel {
	int r;
	int g;
	int b;
	int a;
};

const unsigned int CHANNELS = 4; //R G B A
const unsigned int FLAG = 9;

/**
	Writes a map of Pixel to file
	args:
		pixels: array of pixels that make an image
		width: width of image in pixels
		height: height of image in pixels
*/
int write(char* filename, struct Pixel *pixels, int width, int height) {
	int length = width * height;
	unsigned char* data = malloc(length * CHANNELS * sizeof(unsigned char));
	int currentLocation = 0;
		for(int i = 0; i < length; i++) {
			//Add byte value of pixel and then updates location in buffer.
			data[currentLocation++] = pixels[i].r;
			data[currentLocation++] = pixels[i].g;
			data[currentLocation++] = pixels[i].b;
			data[currentLocation++] = pixels[i].a;
	}

	stbi_write_png(filename, width, height, CHANNELS, data, width * CHANNELS);

	return 0;
}

/**
	Reads image file.
	args:
		width: location to store width of image
		height: location to store height of image
*/
unsigned char*  read(char* filename, int *width, int *height) {
	int n;
	unsigned char *data = stbi_load(filename, width, height, &n, CHANNELS);

	if(data == NULL || n < CHANNELS - 1 /* Must have at least RGB */) {
		printf("%i\n", n);
		printf("Reading: \"%s\" failed. Code: %i", filename, n);
		return NULL;
	}

	printf("%s\n", "Read original image.");
	return data;
}

struct Pixel* mapPixels(unsigned char* data, int width, int height) {
	long l = width * height;
	struct Pixel* pixels = malloc(l * sizeof(struct Pixel));

	long currentLocation = 0;
	for(int i = 0; i < l; i++) {
			//Add byte value of pixel and then updates location in buffer.
			pixels[i].r = data[currentLocation++];
			pixels[i].g = data[currentLocation++];
			pixels[i].b = data[currentLocation++];
			pixels[i].a = data[currentLocation++];
	}

	return pixels;
}

/**
	Encodes string in Pixel map
	args:
		pixels: pixel map to write into
		str: string to write
		width: image width
		height: image height
*/
int encode(struct Pixel* pixels, char* str, int width, int height) {
	if(strlen(str) > (width * height)) {
		printf("%s\n", "Not enough space in file.");
		return 1;
	}

	int i = 0;
	while(str[i] != '\0') {
		pixels[i].a = FLAG;
		pixels[i].b = str[i];
		i++;
	}

	return 0;
}


/**
	Encodes string in Pixel map
	args:
		pixels: pixel map to read from
		width: image width
		height: image height
*/
char* decode(struct Pixel* pixels,  int width, int height) {
	char* buff = malloc(width*height * sizeof(char));
	int i = 0;
	while(pixels[i].a == FLAG) {
		buff[i] = pixels[i].b;
		i++;
	}
	buff[i] = '\0';

	int l = strlen(buff);
	char* str = malloc(l * sizeof(char));

	strcpy(str, buff);
	free(buff);
	return str;
}

int main(int argc, char* argv[]) {
	if (argc < 3) {
		printf("%s\n", "fsteg -r <filename>: Tries to read file.");
		printf("%s\n", "fsteg -w <file name> <\"string\"> <output file name>: Tries to write string to file.");
		return 0;
	}
	

	int w, h;
	struct Pixel* pixels;

	unsigned char* data = read(argv[2], &w, &h);
	if(data == NULL) {
		return 1;
	} else {
		pixels = mapPixels(data, w, h);
	}
	
	if(strcmp(argv[1], "-r") == 0) {
		char* str = decode(pixels, w, h);
		printf("\n%s\n", str);
	}
	else if(strcmp(argv[1], "-w") == 0) {
		if(argc < 5) {
			printf("%s\n", "Writing failed.");
			printf("%s\n", "fsteg -w <file name> <\"string\"> <output file name>: Tries to write string to file.");
			return 1;
		}
		printf("%s\n", "Writing coded image.");
		char* str = argv[3];
		char* filename = argv[4];
		encode(pixels, str, w, w);
		write(filename, pixels, w, h);
		printf("%s\n", "Success.");
	} 
	else {
		printf("%s\n", "fsteg -r <filename>: Tries to read file.");
		printf("%s\n", "fsteg -w <file name> <\"string\"> <output file name>: Tries to write string to file.");
		return 0;
	}

	free(pixels);
	stbi_image_free(data);

  return 0;
}
