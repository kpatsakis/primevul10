bool Jsi_StrIsAlnum(const char *cp)
{
    if (!cp || !*cp) return 0;
    while (*cp)
        if (isalnum(*cp) || *cp == '_')
            cp++;
        else
            return 0;
    return 1;
}