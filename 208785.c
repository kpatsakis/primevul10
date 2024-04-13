void CjfifDecode::GenLookupHuffMask()
{
	unsigned int mask;
	for (unsigned len=0;len<32;len++)
	{
		mask = (1 << (len))-1;
		mask <<= 32-len;
		m_anMaskLookup[len] = mask;
	}
}