#include "extra/extra.h"

void print::m(const char *format, ...) {

    va_list args;
    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void print::d(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stdout, "[DEBUG] ");
    vfprintf(stdout, format, args);
    va_end(args);
    fprintf(stdout, "\n");
}

void print::w(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stderr, "[WARNING] ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void print::e(const char *format, ...) {

    va_list args;
    va_start(args, format);
    fprintf(stderr, "[ERROR] ");
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}

// TODO:: This should return a const char*
std::string print::format(const char *fmt, ...) {
    int size = 512;
    char *buffer = 0;
    buffer = new char[size];
    va_list vl;
    va_start(vl, fmt);
    int nsize = vsnprintf(buffer, size, fmt, vl);
    if(size <= nsize) {
	delete[] buffer;
	buffer = nullptr;
	buffer = new char[nsize+1];
	nsize = vsnprintf(buffer, size, fmt, vl);
    }
    std::string ret(buffer);
    va_end(vl);
    delete[] buffer;
    buffer = nullptr;
    return ret;
}
