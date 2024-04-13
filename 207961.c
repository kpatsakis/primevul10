static void DoStackWalk(logentry *p) THROWSPEC
{
	void *backtr[NEDMALLOC_STACKBACKTRACEDEPTH];
	size_t size;
	char **strings;
	size_t i2;
	size=backtrace(backtr, NEDMALLOC_STACKBACKTRACEDEPTH);
	strings=backtrace_symbols(backtr, size);
	for(i2=0; i2<size; i2++)
	{	// Format can be <file path>(<mangled symbol>+0x<offset>) [<pc>]
		// or can be <file path> [<pc>]
		int start=0, end=strlen(strings[i2]), which=0, idx;
		for(idx=0; idx<end; idx++)
		{
			if(0==which && (' '==strings[i2][idx] || '('==strings[i2][idx]))
			{
				int len=FXMIN(idx-start, (int) sizeof(p->stack[i2].file));
				memcpy(p->stack[i2].file, strings[i2]+start, len);
				p->stack[i2].file[len]=0;
				which=(' '==strings[i2][idx]) ? 2 : 1;
				start=idx+1;
			}
			else if(1==which && ')'==strings[i2][idx])
			{
				FXString functname(strings[i2]+start, idx-start);
				FXint offset=functname.rfind("+0x");
				FXString rawsymbol(functname.left(offset));
				FXString symbol(rawsymbol.length() ? fxdemanglesymbol(rawsymbol, false) : rawsymbol);
				symbol.append(functname.mid(offset));
				int len=FXMIN(symbol.length(), (int) sizeof(p->stack[i2].functname));
				memcpy(p->stack[i2].functname, symbol.text(), len);
				p->stack[i2].functname[len]=0;
				which=2;
			}
			else if(2==which && '['==strings[i2][idx])
			{
				start=idx+1;
				which=3;
			}
			else if(3==which && ']'==strings[i2][idx])
			{
				FXString v(strings[i2]+start+2, idx-start-2);
				p->stack[i2].pc=(void *)(FXuval)v.toULong(0, 16);
			}
		}
	}
	free(strings);
}