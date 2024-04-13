sync_number2ulong(char *chgnrstr)
{
    char *end;
    unsigned long nr;
    nr = strtoul(chgnrstr, &end, 10);
    if (*end == '\0') {
        return (nr);
    } else {
        return SYNC_INVALID_CHANGENUM;
    }
}