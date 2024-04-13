xfree(void *s)
{
    if (s != NULL)
        free(s);
    s = NULL;
}