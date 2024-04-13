DLLEXPORT unsigned long DLLCALL tjBufSizeYUV(int width, int height,
	int subsamp)
{
	return tjBufSizeYUV2(width, 4, height, subsamp);
}