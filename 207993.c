static void DoStackWalk(logentry *p) THROWSPEC
{
	int i,i2;
	HANDLE mythread=(HANDLE) GetCurrentThread();
	STACKFRAME64 sf={ 0 };
	CONTEXT ct={ 0 };
	if(!sym_myprocess)
	{
		DWORD symopts;
		DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(), &sym_myprocess, 0, FALSE, DUPLICATE_SAME_ACCESS);
		symopts=SymGetOptions();
		SymSetOptions(symopts /*| SYMOPT_DEFERRED_LOADS*/ | SYMOPT_LOAD_LINES);
		SymInitialize(sym_myprocess, NULL, TRUE);
		atexit(DeinitSym);
	}
	ct.ContextFlags=CONTEXT_FULL;

	// Use RtlCaptureContext() if we have it as it saves an exception throw
	if((VOID (WINAPI *)(PCONTEXT)) -1==RtlCaptureContextAddr)
		RtlCaptureContextAddr=(VOID (WINAPI *)(PCONTEXT)) GetProcAddress(GetModuleHandle(L"kernel32"), "RtlCaptureContext");
	if(RtlCaptureContextAddr)
		RtlCaptureContextAddr(&ct);
	else
	{	// This is nasty, but it works
		__try
		{
			int *foo=0;
			*foo=78;
		}
		__except (ExceptionFilter(GetExceptionCode(), GetExceptionInformation(), &ct))
		{
		}
	}

	sf.AddrPC.Mode=sf.AddrStack.Mode=sf.AddrFrame.Mode=AddrModeFlat;
#if !(defined(_M_AMD64) || defined(_M_X64))
	sf.AddrPC.Offset   =ct.Eip;
	sf.AddrStack.Offset=ct.Esp;
	sf.AddrFrame.Offset=ct.Ebp;
#else
	sf.AddrPC.Offset   =ct.Rip;
	sf.AddrStack.Offset=ct.Rsp;
	sf.AddrFrame.Offset=ct.Rbp; // maybe Rdi?
#endif
	for(i2=0; i2<NEDMALLOC_STACKBACKTRACEDEPTH; i2++)
	{
		IMAGEHLP_MODULE64 ihm={ sizeof(IMAGEHLP_MODULE64) };
		char temp[MAX_PATH+sizeof(IMAGEHLP_SYMBOL64)];
		IMAGEHLP_SYMBOL64 *ihs;
		IMAGEHLP_LINE64 ihl={ sizeof(IMAGEHLP_LINE64) };
		DWORD64 offset;
		if(!StackWalk64(
#if !(defined(_M_AMD64) || defined(_M_X64))
			IMAGE_FILE_MACHINE_I386,
#else
			IMAGE_FILE_MACHINE_AMD64,
#endif
			sym_myprocess, mythread, &sf, &ct, NULL, SymFunctionTableAccess64, GetModBase, NULL))
			break;
		if(0==sf.AddrPC.Offset)
			break;
		i=i2;
		if(i)	// Skip first entry relating to this function
		{
			DWORD lineoffset=0;
			p->stack[i-1].pc=(void *)(size_t) sf.AddrPC.Offset;
			if(SymGetModuleInfo64(sym_myprocess, sf.AddrPC.Offset, &ihm))
			{
				char *leaf;
				leaf=strrchr(ihm.ImageName, '\\');
				if(!leaf) leaf=ihm.ImageName-1;
				COPY_STRING(p->stack[i-1].module, leaf+1, sizeof(p->stack[i-1].module));
			}
			else strcpy(p->stack[i-1].module, "<unknown>");
			//fxmessage("WARNING: SymGetModuleInfo64() returned error code %d\n", GetLastError());
			memset(temp, 0, MAX_PATH+sizeof(IMAGEHLP_SYMBOL64));
			ihs=(IMAGEHLP_SYMBOL64 *) temp;
			ihs->SizeOfStruct=sizeof(IMAGEHLP_SYMBOL64);
			ihs->Address=sf.AddrPC.Offset;
			ihs->MaxNameLength=MAX_PATH;

			if(SymGetSymFromAddr64(sym_myprocess, sf.AddrPC.Offset, &offset, ihs))
			{
				COPY_STRING(p->stack[i-1].functname, ihs->Name, sizeof(p->stack[i-1].functname));
				if(strlen(p->stack[i-1].functname)<sizeof(p->stack[i-1].functname)-8)
				{
					sprintf(strchr(p->stack[i-1].functname, 0), " +0x%x", offset);
				}
			}
			else
				strcpy(p->stack[i-1].functname, "<unknown>");
			if(SymGetLineFromAddr64(sym_myprocess, sf.AddrPC.Offset, &lineoffset, &ihl))
			{
				char *leaf;
				p->stack[i-1].lineno=ihl.LineNumber;

				leaf=strrchr(ihl.FileName, '\\');
				if(!leaf) leaf=ihl.FileName-1;
				COPY_STRING(p->stack[i-1].file, leaf+1, sizeof(p->stack[i-1].file));
			}
			else
				strcpy(p->stack[i-1].file, "<unknown>");
		}
	}
}