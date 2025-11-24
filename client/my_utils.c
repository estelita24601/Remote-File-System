#include "my_utils.h"

#include <string.h>

bool equals(const char* a, const char* b) {
    if (strcmp(a, b) == 0) return true;
    return false;
}