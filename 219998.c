hdrblob hdrblobFree(hdrblob blob)
{
    if (blob) {
	free(blob->ei);
	free(blob);
    }
    return NULL;
}