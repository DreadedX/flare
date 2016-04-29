/** @todo Figure out which of these are really needed */
// Standard library
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <assert.h>
#include <algorithm>
#include <ctime>

// System libraries
#include <png.h>
#include <zlib.h>

#include "jsoncons/json.hpp"

#include "tinyobjloader/tiny_obj_loader.h"

// Needed to be able to store assets in the same format as flare
#include "flare/flare.h"

// Debug
// #ifndef NDEBUG
//     #include "debug_new.h"
// #endif

#include "extra/extra.h"
#include "flux/flux.h"

#include "plugin_png.h"
#include "plugin.h"

png::Data png::read(const char *name) {

    Data data;

    png_structp png_ptr;
    png_infop info_ptr;
    uint sig_read = 0;
    int color_type;
    int interlace_type;
    FILE *fp;

    if ((fp = fopen(name, "rb")) == NULL) {
	return data;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
	fclose(fp);
	return data;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
	fclose(fp);
	png_destroy_read_struct(&png_ptr, NULL, NULL);
	return data;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	return data;
    }

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, sig_read);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

    png_uint_32 width;
    png_uint_32 height;
    int bit_depth;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, &interlace_type, NULL, NULL);

    uint row_bytes = png_get_rowbytes(png_ptr, info_ptr);
    data.pixels = (byte*) malloc(row_bytes * height);
    data.size = row_bytes * height;

    png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
    for (uint i = 0; i < height; i++) {
	// note that png is ordered top to
	// bottom, but OpenGL expect it bottom to top
	// so the order or swapped
	memcpy(data.pixels+(row_bytes * (height - 1 -i)), row_pointers[i], row_bytes);
    }

    data.width = width;
    data.height = height;

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    fclose(fp);

    data.bytesPerPixel = data.size / (data.width * data.height);

    return data;
}

void load(std::string filePath, flux::FileWrite *file) {

	png::Data data = png::read(filePath.c_str());

	file->extraSize = sizeof(int) * 2 + sizeof(byte);
	file->extra = new byte[file->extraSize];

	for (uint i = 0; i < sizeof(int); ++i) {

		file->extra[i] = data.width >> (i*8);
	}
	for (uint i = 0; i < sizeof(int); ++i) {

		file->extra[i+sizeof(int)] = data.height >> (i*8);
	}
	file->extra[sizeof(int)*2] = data.bytesPerPixel;

	// file->data = data.pixels;
	file->dataSize = data.size;
	file->data = new byte[file->dataSize];
	for (int y = 0; y < data.height; y++) {
		for (int x = 0; x < data.width*data.bytesPerPixel; x++) {

			// file->data[x + y * (data.width*data.bytesPerPixel)] = data.pixels[x + (data.height-y-1) * (data.width*data.bytesPerPixel)];
			file->data[x + y * (data.width*data.bytesPerPixel)] = data.pixels[x + y * (data.width*data.bytesPerPixel)];
		}
	}
}

Plugin plugin("PNG Plugin", "Allows fluxuate to pack png files in a format compatible with Flare", load);

extern "C" {

	Plugin getPlugin() {

		return plugin;
	}
}
