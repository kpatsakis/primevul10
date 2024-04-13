LZWPreEncode(TIFF* tif, uint16 s)
{
	LZWCodecState *sp = EncoderState(tif);

	(void) s;
	assert(sp != NULL);

	if( sp->enc_hashtab == NULL )
        {
            tif->tif_setupencode( tif );
        }

	sp->lzw_nbits = BITS_MIN;
	sp->lzw_maxcode = MAXCODE(BITS_MIN);
	sp->lzw_free_ent = CODE_FIRST;
	sp->lzw_nextbits = 0;
	sp->lzw_nextdata = 0;
	sp->enc_checkpoint = CHECK_GAP;
	sp->enc_ratio = 0;
	sp->enc_incount = 0;
	sp->enc_outcount = 0;
	/*
	 * The 4 here insures there is space for 2 max-sized
	 * codes in LZWEncode and LZWPostDecode.
	 */
	sp->enc_rawlimit = tif->tif_rawdata + tif->tif_rawdatasize-1 - 4;
	cl_hash(sp);		/* clear hash table */
	sp->enc_oldcode = (hcode_t) -1;	/* generates CODE_CLEAR in LZWEncode */
	return (1);
}