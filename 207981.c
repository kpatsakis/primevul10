static NOINLINE void GetThreadCache_cold1(nedpool *RESTRICT *RESTRICT p) THROWSPEC
{
	*p=&syspool;
	if(!syspool.threads) InitPool(&syspool, 0, -1);
}