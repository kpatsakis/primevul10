R_API bool r_str_glob(const char* str, const char *glob) {
        const char* cp = NULL, *mp = NULL;
        if (!glob || !strcmp (glob, "*")) {
                return true;
        }
        if (!strchr (glob, '*')) {
                if (*glob == '^') {
                        glob++;
                        while (*str) {
                                if (*glob != *str) {
                                        return false;
                                }
                                if (!*++glob) {
                                        return true;
                                }
                                str++;
                        }
                } else {
                        return strstr (str, glob) != NULL;
                }
        }
        if (*glob == '^') {
                glob++;
        }
        while (*str && (*glob != '*')) {
                if (*glob != *str) {
                        return false;
                }
                glob++;
                str++;
        }
        while (*str) {
                if (*glob == '*') {
                        if (!*++glob) {
                                return true;
                        }
                        mp = glob;
                        cp = str + 1;
                } else if (*glob == *str) {
                        glob++;
                        str++;
                } else {
                        glob = mp;
                        str = cp++;
                }
        }
        while (*glob == '*') {
                ++glob;
        }
        return (*glob == '\x00');
}