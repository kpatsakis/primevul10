void mt_init(mtrand *mt, uint32_t seed) {
  int i;
  mt->mt_buffer_[0] = seed;
  mt->mt_index_ = MT_LEN;
  for (i = 1; i < MT_LEN; i++) {
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, MSBs of the seed affect   */
    /* only MSBs of the array mt[].                        */
    /* 2002/01/09 modified by Makoto Matsumoto             */
    mt->mt_buffer_[i] =
	(1812433253UL * (mt->mt_buffer_[i-1] ^
			 (mt->mt_buffer_[i-1] >> 30)) + i);
  }
}