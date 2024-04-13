void *nedgetvalue(nedpool **p, void *mem) THROWSPEC
{
	nedpool *np=0;
	mstate fm=nedblkmstate(mem);
	if(!fm || !fm->extp) return 0;
	np=(nedpool *) fm->extp;
	if(p) *p=np;
	return np->uservalue;
}