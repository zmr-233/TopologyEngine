#include "shader.hpp"

int* get_status() {
    static int status = 0;
    return &status;
}

char* get_status_char() {
    static char infoLog[infoLogSize];
    return infoLog;
}