DLLEXPORT void DLLCALL tjFree(unsigned char *buf)
{
	if(buf) free(buf);
}