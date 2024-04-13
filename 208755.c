void CjfifDecode::UnByteSwap4(unsigned nVal,unsigned &nByte0,unsigned &nByte1,unsigned &nByte2,unsigned &nByte3)
{
	if (m_nImgExifEndian == 0) {
		// Little Endian
		nByte3 = (nVal & 0xFF000000) >> 24;
		nByte2 = (nVal & 0x00FF0000) >> 16;
		nByte1 = (nVal & 0x0000FF00) >> 8;
		nByte0 = (nVal & 0x000000FF);
	} else {
		// Big Endian
		nByte0 = (nVal & 0xFF000000) >> 24;
		nByte1 = (nVal & 0x00FF0000) >> 16;
		nByte2 = (nVal & 0x0000FF00) >> 8;
		nByte3 = (nVal & 0x000000FF);
	}
}