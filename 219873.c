DLLEXPORT unsigned char *DLLCALL tjAlloc(int bytes)
{
	return (unsigned char *)malloc(bytes);
}