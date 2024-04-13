hdrblob hdrblobCreate(void)
{
    hdrblob blob = xcalloc(1, sizeof(*blob));
    return blob;
}