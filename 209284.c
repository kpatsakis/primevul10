bool jsi_StrIsBalanced(char *str) {
    int cnt = 0, quote = 0;
    char *cp = str;
    while (*cp) {
        switch (*cp) {
        case '\\':
            cp++;
            break;
        case '{': case '(': case '[':
            cnt++;
            break;
        case '\'': case '\"':
            quote++;
            break;
        case '}': case ')': case ']':
            cnt--;
            break;
        }
        if (*cp == 0)
            break;
        cp++;
    }
    return ((quote%2) == 0 && cnt <= 0);
}