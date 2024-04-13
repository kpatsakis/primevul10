static int datain(void *data, int size)
{
    if (fread(data, 1, size, g_fin) != size)
        return ERR_FAIL;
    return size;
}