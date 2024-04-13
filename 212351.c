static int stringin(char *txt, int sizemax)
{
    int size;
    for (size = 0; size < sizemax; size++)
    {
        if (fread(txt + size, 1, 1, g_fin) != 1)
            return ERR_FAIL;
        if (!txt[size])
            break;
    }
    txt[sizemax-1] = '\0';

    return size;
}