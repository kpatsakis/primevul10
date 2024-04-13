static const char *JsiCharsetMatch(const char *pattern, int c, int flags)
{
    int inot = 0;
    int pchar;
    int match = 0;
    int nocase = 0;

    if (flags & JSI_CMP_NOCASE) {
        nocase++;
        c = toupper(c);
    }

    if (flags & JSI_CMP_CHARSET_SCAN) {
        if (*pattern == '^') {
            inot++;
            pattern++;
        }

        /* Special case. If the first char is ']', it is part of the set */
        if (*pattern == ']') {
            goto first;
        }
    }

    while (*pattern && *pattern != ']') {
        /* Exact match */
        if (pattern[0] == '\\') {
first:
            pattern += Jsi_UtfToUniCharCase(pattern, &pchar, nocase);
        }
        else {
            /* Is this a range? a-z */
            int start;
            int end;
            pattern += Jsi_UtfToUniCharCase(pattern, &start, nocase);
            if (pattern[0] == '-' && pattern[1]) {
                /* skip '-' */
                pattern += Jsi_UtfToUniChar(pattern, &pchar);
                pattern += Jsi_UtfToUniCharCase(pattern, &end, nocase);

                /* Handle reversed range too */
                if ((c >= start && c <= end) || (c >= end && c <= start)) {
                    match = 1;
                }
                continue;
            }
            pchar = start;
        }

        if (pchar == c) {
            match = 1;
        }
    }
    if (inot) {
        match = !match;
    }

    return match ? pattern : NULL;
}