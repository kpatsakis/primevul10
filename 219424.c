sync_number2int(char *chgnrstr)
{
    char *end;
    int nr;
    nr = (int)strtoul(chgnrstr, &end, 10);
    if (*end == '\0') {
        return (nr);
    } else {
        return (-1);
    }
}