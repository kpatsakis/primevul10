prefix_is_simple(const char *pstr)
{
    int i;
    char c;

    for (i = 0; (c = pstr[i]) != 0; i++) {
        if (!(c == '-' || c == '_' || (c >= '0' && c <= '9') ||
              (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')))
            return false;
    }
    return true;
}