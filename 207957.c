void neddestroysyspool() THROWSPEC
{
	nedpool *p=&syspool;
	int n;
#if USE_LOCKS
	ACQUIRE_LOCK(&p->mutex);
#endif
	DestroyCaches(p);
	for(n=0; p->m[n]; n++)
	{
#if USE_ALLOCATOR==1
		destroy_mspace(p->m[n]);
#endif
		p->m[n]=0;
	}
	/* Render syspool unusable */
	for(n=0; n<THREADCACHEMAXCACHES; n++)
		p->caches[n]=(threadcache *)(size_t)(sizeof(size_t)>4 ? 0xdeadbeefdeadbeefULL : 0xdeadbeefUL);
	for(n=0; n<MAXTHREADSINPOOL+1; n++)
		p->m[n]=(mstate)(size_t)(sizeof(size_t)>4 ? 0xdeadbeefdeadbeefULL : 0xdeadbeefUL);
	if(TLSFREE(p->mycache)) abort();
#if USE_LOCKS
	RELEASE_LOCK(&p->mutex);
	DESTROY_LOCK(&p->mutex);
#endif
}