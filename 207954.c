static timeCount GetTimestamp()
{
	static LARGE_INTEGER ticksPerSec;
	static double scalefactor;
	static timeCount baseCount;
	LARGE_INTEGER val;
	timeCount ret;
	if(!scalefactor)
	{
		if(QueryPerformanceFrequency(&ticksPerSec))
			scalefactor=ticksPerSec.QuadPart/1000000000000.0;
		else
			scalefactor=1;
	}
	if(!QueryPerformanceCounter(&val))
		return (timeCount) GetTickCount() * 1000000000;
	ret=(timeCount) (val.QuadPart/scalefactor);
	if(!baseCount) baseCount=ret;
	return ret-baseCount;
}