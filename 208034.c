split( char* str, const char* delim)
{
        int size = 0;
        char** splitted = NULL;
        char *tmp = NULL;
        tmp = strtok(str, delim);
        while (tmp != NULL) {
                splitted = (char**) realloc(splitted, sizeof(char**) * (size+1));
                if (splitted != NULL) {
                        splitted[size] = tmp;
                } else {
                        return(NULL);
                }
                tmp = strtok(NULL, delim);
                size++;
        }
        free(tmp);
        tmp = NULL;
        /* add null at end of array to help ptrarray_length */
        splitted = (char**) realloc(splitted, sizeof(char**) * (size+1));
        if (splitted != NULL) {
                splitted[size] = NULL;
        } else {
                return(NULL);
        }

        return splitted;
}