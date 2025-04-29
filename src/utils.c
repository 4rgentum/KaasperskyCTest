#include "../include/utils.h"
#include <string.h>

int matches_pattern(const char *filename, const char *pattern) {
    if (!pattern || !filename) return 0;
    
    const char *f = filename;
    const char *p = pattern;
    const char *f_ptr, *p_ptr;

    while (*f) {
        if (*p == '*') {
            p_ptr = p++;
            f_ptr = f;
        } else if (*p == '?' || *p == *f) {
            p++;
            f++;
        } else if (p_ptr != NULL) {
            p = p_ptr + 1;
            f = ++f_ptr;
        } else {
            return 0;
        }
    }

    while (*p == '*') p++;
    return (*p == '\0');
}