/*
    Convenient string implementation that uses vector.c
*/
#include "sstring.h"
#include "vector.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <string.h>

struct sstring {
    char * str;
};

sstring *cstr_to_sstring(const char *input) {
    sstring *sstr = malloc(sizeof(sstring));
    if (!sstr) {
        perror("malloc for sstr failed in cstr_to_sstring");
        exit(1);
    }
    sstr -> str = malloc((strlen(input) + 1) * sizeof(char));
    if (!sstr -> str) {
        perror("malloc for sstr -> str failed in cstr_to_sstring");
        free(sstr);
        exit(1);
    }
    strcpy(sstr -> str, input);
    return sstr;
}

char *sstring_to_cstr(sstring *input) {    
    char * output = malloc(sizeof(char*));
    strcpy(output, input->str);
    return output;
}

int sstring_append(sstring *this, sstring *addition) {
    size_t totalLen = strlen(this -> str) + strlen(addition -> str);
    this -> str = realloc(this -> str, totalLen + 1);
    strcat(this -> str, addition -> str);
    return totalLen;
}

vector *sstring_split(sstring *this, char delimiter) {
    vector * vec = string_vector_create();
    int begin = 0;
    int end = 0;
    while (end <= (int)strlen(this -> str) && begin <= (int)strlen(this -> str)) {
        if (this -> str[end] == delimiter || end == (int)strlen(this -> str)) { 
            char * substr = malloc((end - begin + 1) * sizeof(char));
            strncpy(substr, this -> str + begin, end - begin);
            substr[end - begin] = '\0';
            vector_push_back(vec, substr);
            end ++;
            begin = end;
        } else {
            end ++;
        }
    }
    return vec;
}

int sstring_substitute(sstring *this, size_t offset, char *target,
                       char *substitution) {
    if (strlen(target) > strlen(this -> str)) return -1;
    int ind = (int)offset;
    char * substr = malloc((strlen(target) + 1) * sizeof(char));
    while (ind <= (int)(strlen(this -> str) - strlen(target))) {
        strncpy(substr, &this -> str[ind], strlen(target));
        substr[strlen(target)] = '\0';
        if (strcmp(substr, target) == 0) {
            size_t afterSub = strlen(this->str) - (ind + strlen(target));
            size_t newLen = strlen(this->str) - strlen(target) + strlen(substitution);
            this -> str = realloc(this -> str, (newLen + 1) * sizeof(char));
            if (!this -> str) {
                perror("realloc failed in sstring_substitute");
                free(substr);
                exit(1);
            }

            memmove(&this->str[ind + strlen(substitution)], &this->str[ind + strlen(target)], afterSub + 1);
            strncpy(&this->str[ind], substitution, strlen(substitution));
            free(substr);
            return 0;
        }
        ind ++;
    }
    free(substr);
    return -1;
}

char *sstring_slice(sstring *this, int start, int end) {
    char * output = malloc((end - start + 1) * sizeof(char));
    strncpy(output, &this -> str[start], end - start);
    output[end - start] = '\0';
    return output;
}

void sstring_destroy(sstring *this) {
    if (this == NULL) return;
    if (this -> str != NULL) {
        free(this -> str);
        this -> str = NULL;
    }
    free(this);
    this = NULL;
}
