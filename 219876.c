DLLEXPORT tjhandle DLLCALL tjInitCompress(void)
{
	tjinstance *this=NULL;
	if((this=(tjinstance *)malloc(sizeof(tjinstance)))==NULL)
	{
		snprintf(errStr, JMSG_LENGTH_MAX,
			"tjInitCompress(): Memory allocation failure");
		return NULL;
	}
	MEMZERO(this, sizeof(tjinstance));
	return _tjInitCompress(this);
}