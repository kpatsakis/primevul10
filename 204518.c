BIGNUM *bn_expand2(BIGNUM *b, int words)
	{
	bn_check_top(b);

	if (words > b->dmax)
		{
		BN_ULONG *a = bn_expand_internal(b, words);
		if(!a) return NULL;
		if(b->d) OPENSSL_free(b->d);
		b->d=a;
		b->dmax=words;
		}

/* None of this should be necessary because of what b->top means! */
#if 0
	/* NB: bn_wexpand() calls this only if the BIGNUM really has to grow */
	if (b->top < b->dmax)
		{
		int i;
		BN_ULONG *A = &(b->d[b->top]);
		for (i=(b->dmax - b->top)>>3; i>0; i--,A+=8)
			{
			A[0]=0; A[1]=0; A[2]=0; A[3]=0;
			A[4]=0; A[5]=0; A[6]=0; A[7]=0;
			}
		for (i=(b->dmax - b->top)&7; i>0; i--,A++)
			A[0]=0;
		assert(A == &(b->d[b->dmax]));
		}
#endif
	bn_check_top(b);
	return b;
	}