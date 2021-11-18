#include "libs.h"

int namecmp(const char *word1, const char *word2) {
    // This function is strcmp that doesn't give a damn about letters size.
    while (*word1)
    {
        if (toupper(*word1) != toupper(*word2)) {
            break;
        }
 
        word1++;
        word2++;
    }
 
    return *(const unsigned char*)word1 - *(const unsigned char*)word2;
}
