LZWPreDecode(TIFF* tif, uint16 s)
{
	static const char module[] = "LZWPreDecode";
	LZWCodecState *sp = DecoderState(tif);

	(void) s;
	assert(sp != NULL);
	if( sp->dec_codetab == NULL )
        {
            tif->tif_setupdecode( tif );
	    if( sp->dec_codetab == NULL )
		return (0);
        }

	/*
	 * Check for old bit-reversed codes.
	 */
	if (tif->tif_rawcc >= 2 &&
	    tif->tif_rawdata[0] == 0 && (tif->tif_rawdata[1] & 0x1)) {
#ifdef LZW_COMPAT
		if (!sp->dec_decode) {
			TIFFWarningExt(tif->tif_clientdata, module,
			    "Old-style LZW codes, convert file");
			/*
			 * Override default decoding methods with
			 * ones that deal with the old coding.
			 * Otherwise the predictor versions set
			 * above will call the compatibility routines
			 * through the dec_decode method.
			 */
			tif->tif_decoderow = LZWDecodeCompat;
			tif->tif_decodestrip = LZWDecodeCompat;
			tif->tif_decodetile = LZWDecodeCompat;
			/*
			 * If doing horizontal differencing, must
			 * re-setup the predictor logic since we
			 * switched the basic decoder methods...
			 */
			(*tif->tif_setupdecode)(tif);
			sp->dec_decode = LZWDecodeCompat;
		}
		sp->lzw_maxcode = MAXCODE(BITS_MIN);
#else /* !LZW_COMPAT */
		if (!sp->dec_decode) {
			TIFFErrorExt(tif->tif_clientdata, module,
			    "Old-style LZW codes not supported");
			sp->dec_decode = LZWDecode;
		}
		return (0);
#endif/* !LZW_COMPAT */
	} else {
		sp->lzw_maxcode = MAXCODE(BITS_MIN)-1;
		sp->dec_decode = LZWDecode;
	}
	sp->lzw_nbits = BITS_MIN;
	sp->lzw_nextbits = 0;
	sp->lzw_nextdata = 0;

	sp->dec_restart = 0;
	sp->dec_nbitsmask = MAXCODE(BITS_MIN);
#ifdef LZW_CHECKEOS
	sp->dec_bitsleft = 0;
#endif
	sp->dec_free_entp = sp->dec_codetab + CODE_FIRST;
	/*
	 * Zero entries that are not yet filled in.  We do
	 * this to guard against bogus input data that causes
	 * us to index into undefined entries.  If you can
	 * come up with a way to safely bounds-check input codes
	 * while decoding then you can remove this operation.
	 */
	_TIFFmemset(sp->dec_free_entp, 0, (CSIZE-CODE_FIRST)*sizeof (code_t));
	sp->dec_oldcodep = &sp->dec_codetab[-1];
	sp->dec_maxcodep = &sp->dec_codetab[sp->dec_nbitsmask-1];
	return (1);
}