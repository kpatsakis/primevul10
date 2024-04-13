void nedpsetvalue(nedpool *p, void *v) THROWSPEC
{
	if(!p) { p=&syspool; if(!syspool.threads) InitPool(&syspool, 0, -1); }
	p->uservalue=v;
}