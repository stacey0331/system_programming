/*
    Convert a string into camel case.
    Example input: "The Heisenbug is an incredible creature. 
                    Facenovel servers get their power from its 
                    indeterminism. Code smell can be ignored with
                     INCREDIBLE use of air freshener. God objects
                      are the new religion."
    Example output: ["theHeisenbugIsAnIncredibleCreature",
                    "facenovelServersGetTheirPowerFromItsIndeterminism",
                    "codeSmellCanBeIgnoredWithIncredibleUseOfAirFreshener",
                    "godObjectsAreTheNewReligion",
                    NULL]
*/

#include "camelCaser.h"
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>

/// @brief process each array element. remove white space and convert to camel cases
/// @param start start of sentence
/// @param end end pf sentence
/// @param len length of the sentence without whitespace
/// @return correct camelcase sentence of that element
char *toCamel(char *start, const char *end, const int len) {
    if (!start || !end) return NULL;
    char * output = malloc((len + 1) * sizeof(char));
    if (!output) return NULL;
    int ind = 0;
    // get rid of front spaces for easier processing later
    while (start != NULL && isspace(*start) && start != end) start ++;

    while (start != end) { // end points to a punctuation
        if (isspace(*start)) { // current is space
            if ((start + 1) == end) {
                start ++;
                continue;
            }

            if (isspace(*(start + 1))) { // next is space
                start ++;
                continue;
            } else if (isalpha(*(start + 1))) { // next is alpha
                start ++;
                output[ind] = toupper(*start);
                start ++;
                ind ++; 
            } else { // next is not space nor alpha 
                start ++; // skip space
                output[ind] = *start;
                start ++;
                ind ++;
                while (start != NULL && start != end && !isspace(*start) && !isalpha(*start)) {
                    output[ind] = *start;
                    start ++;
                    ind ++;
                }

                if (start != NULL && isalpha(*start)) {
                    output[ind] = toupper(*start);
                    ind ++;
                    start ++;
                }
            }
        } else {
            output[ind] = tolower(*start);
            start ++;
            ind ++;
        }
    }
    output[ind] = '\0';
    return output;
}

char **camel_caser(const char *input_str) {
    if (input_str == NULL) return NULL;

    // count number of sentences (length of output array)
    char * inputPtr = (char *)input_str;
    int numOfSentences = 0;
    while (*inputPtr != '\0') {
        if (ispunct(*inputPtr)) {
            numOfSentences ++;
        }
        inputPtr ++;
    }

    char ** output = malloc((numOfSentences + 1) * sizeof(char *));
    
    int outputInd = 0;
    char * begins = (char *)input_str;
    char * ends = (char *)input_str;
    int sentenceLen = 0;
    while (outputInd < numOfSentences && *ends != '\0') {
        if (ispunct(*ends)) {
            output[outputInd] = toCamel(begins, ends, sentenceLen + 1);
            sentenceLen = 0;
            outputInd ++;
            ends ++; // skip the current punctuation
            begins = ends;
        } else {
            if (!isspace(*ends)) {
                sentenceLen ++;
            }
            ends ++;
        }
    }
    output[numOfSentences] = NULL;
    return output;
}

void destroy(char **result) {
    char ** resultcpy = result;
    while (*resultcpy) {
        free(*resultcpy);
        resultcpy ++;
    }
    free(result);
    result = NULL;
    return;
}
