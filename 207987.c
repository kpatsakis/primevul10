static LPVOID ChkedTlsGetValue(DWORD idx)
{
	LPVOID ret=TlsGetValue(idx);
	assert(S_OK==GetLastError());
	return ret;
}