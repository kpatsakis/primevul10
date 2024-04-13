set_field_split_str(char *arg, char **key, char **value, char **delim)
{
    char *value_end;

    *value = arg;
    value_end = strstr(arg, "->");
    *key = value_end + strlen("->");
    if (delim) {
        *delim = value_end;
    }

    if (!value_end) {
        return xasprintf("%s: missing `->'", arg);
    }
    if (strlen(value_end) <= strlen("->")) {
        return xasprintf("%s: missing field name following `->'", arg);
    }

    return NULL;
}