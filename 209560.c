char *jsi_TrimStr(char *str) {
    while (isspace(*str)) str++;
    if (!str) return str;
    int len = Jsi_Strlen(str);
    while (--len>=0 && isspace(str[len]))
        str[len] = 0;
    return str;
}