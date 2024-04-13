strlen_sigsafe(const char *s)
{
    size_t len;
    for (len = 0; s[len]; len++);
    return len;
}