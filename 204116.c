find_resource (WinLibrary *fi, const char *type, const char *name, const char *language, int *level)
{
    WinResource *wr;

    *level = 0;
    if (type == NULL)
        return NULL;
    wr = find_with_resource_array(fi, NULL, type);
    if (wr == NULL || !wr->is_directory)
        return wr;

    *level = 1;
    if (name == NULL)
        return wr;
    wr = find_with_resource_array(fi, wr, name);
    if (wr == NULL || !wr->is_directory)
        return wr;

    *level = 2;
    if (language == NULL)
        return wr;
    wr = find_with_resource_array(fi, wr, language);
    return wr;
}