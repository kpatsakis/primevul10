static DWORD64 __stdcall GetModBase(HANDLE hProcess, DWORD64 dwAddr) THROWSPEC
{
	DWORD64 modulebase;
	// Try to get the module base if already loaded, otherwise load the module
	modulebase=SymGetModuleBase64(hProcess, dwAddr);
	if(modulebase)
		return modulebase;
	else
	{
		MEMORY_BASIC_INFORMATION stMBI ;
		if ( 0 != VirtualQueryEx ( hProcess, (LPCVOID)(size_t)dwAddr, &stMBI, sizeof(stMBI)))
		{
			int n;
			DWORD dwPathLen=0, dwNameLen=0 ;
			TCHAR szFile[ MAX_PATH ], szModuleName[ MAX_PATH ] ;
			MODULEINFO mi={0};
			dwPathLen = GetModuleFileName ( (HMODULE) stMBI.AllocationBase , szFile, MAX_PATH );
			dwNameLen = GetModuleBaseName (hProcess, (HMODULE) stMBI.AllocationBase , szModuleName, MAX_PATH );
			for(n=dwNameLen; n>0; n--)
			{
				if(szModuleName[n]=='.')
				{
					szModuleName[n]=0;
					break;
				}
			}
			if(!GetModuleInformation(hProcess, (HMODULE) stMBI.AllocationBase, &mi, sizeof(mi)))
			{
				//fxmessage("WARNING: GetModuleInformation() returned error code %d\n", GetLastError());
			}
			if(!SymLoadModule64 ( hProcess, NULL, (PSTR)( (dwPathLen) ? szFile : 0), (PSTR)( (dwNameLen) ? szModuleName : 0),
				(DWORD64) mi.lpBaseOfDll, mi.SizeOfImage))
			{
				//fxmessage("WARNING: SymLoadModule64() returned error code %d\n", GetLastError());
			}
			//fxmessage("%s, %p, %x, %x\n", szFile, mi.lpBaseOfDll, mi.SizeOfImage, (DWORD) mi.lpBaseOfDll+mi.SizeOfImage);
			modulebase=SymGetModuleBase64(hProcess, dwAddr);
			return modulebase;
		}
	}
	return 0;
}