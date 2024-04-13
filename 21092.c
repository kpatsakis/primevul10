LZWPostEncode(TIFF* tif)
{
	register LZWCodecState *sp = EncoderState(tif);
	uint8* op = tif->tif_rawcp;
	long nextbits = sp->lzw_nextbits;
	unsigned long nextdata = sp->lzw_nextdata;
	long outcount = sp->enc_outcount;
	int nbits = sp->lzw_nbits;

	if (op > sp->enc_rawlimit) {
		tif->tif_rawcc = (tmsize_t)(op - tif->tif_rawdata);
		if( !TIFFFlushData1(tif) )
                    return 0;
		op = tif->tif_rawdata;
	}
	if (sp->enc_oldcode != (hcode_t) -1) {
                int free_ent = sp->lzw_free_ent;

		PutNextCode(op, sp->enc_oldcode);
		sp->enc_oldcode = (hcode_t) -1;
                free_ent ++;

                if (free_ent == CODE_MAX-1) {
                        /* table is full, emit clear code and reset */
                        outcount = 0;
                        PutNextCode(op, CODE_CLEAR);
                        nbits = BITS_MIN;
                } else {
                        /*
                        * If the next entry is going to be too big for
                        * the code size, then increase it, if possible.
                        */
                        if (free_ent > sp->lzw_maxcode) {
                                nbits++;
                                assert(nbits <= BITS_MAX);
                        }
                }
	}
	PutNextCode(op, CODE_EOI);
        /* Explicit 0xff masking to make icc -check=conversions happy */
	if (nextbits > 0) 
		*op++ = (unsigned char)((nextdata << (8-nextbits))&0xff);
	tif->tif_rawcc = (tmsize_t)(op - tif->tif_rawdata);
	return (1);
}