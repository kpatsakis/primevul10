cpContigBufToSeparateBuf(uint8* out, uint8* in,
    uint32 rows, uint32 cols, int outskew, int inskew, tsample_t spp,
    int bytes_per_sample )
{
	while (rows-- > 0) {
		uint32 j = cols;
		while (j-- > 0)
		{
			int n = bytes_per_sample;

			while( n-- ) {
				*out++ = *in++;
			}
			in += (spp-1) * bytes_per_sample;
		}
		out += outskew;
		in += inskew;
	}
}