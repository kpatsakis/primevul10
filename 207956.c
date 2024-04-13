void neddisablethreadcache(nedpool *p) THROWSPEC
{
	nedtrimthreadcache(p, 1);
}