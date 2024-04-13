static FORCEINLINE NEDMALLOCNOALIASATTR unsigned int size2binidx(size_t _size) THROWSPEC
{	/* 8=1000	16=10000	20=10100	24=11000	32=100000	48=110000	4096=1000000000000 */
	unsigned int topbit, size=(unsigned int)(_size>>4);
	/* 16=1		20=1	24=1	32=10	48=11	64=100	96=110	128=1000	4096=100000000 */

#if defined(__GNUC__)
        topbit = sizeof(size)*__CHAR_BIT__ - 1 - __builtin_clz(size);
#elif defined(_MSC_VER) && _MSC_VER>=1300
	{
            unsigned long bsrTopBit;

            _BitScanReverse(&bsrTopBit, size);

            topbit = bsrTopBit;
        }
#else
#if 0
	union {
		unsigned asInt[2];
		double asDouble;
	};
	int n;

	asDouble = (double)size + 0.5;
	topbit = (asInt[!FOX_BIGENDIAN] >> 20) - 1023;
#else
	{
		unsigned int x=size;
		x = x | (x >> 1);
		x = x | (x >> 2);
		x = x | (x >> 4);
		x = x | (x >> 8);
		x = x | (x >>16);
		x = ~x;
		x = x - ((x >> 1) & 0x55555555);
		x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
		x = (x + (x >> 4)) & 0x0F0F0F0F;
		x = x + (x << 8);
		x = x + (x << 16);
		topbit=31 - (x >> 24);
	}
#endif
#endif
	return topbit;
}