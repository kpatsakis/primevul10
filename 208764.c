CString CjfifDecode::DecodeIccDateTime(unsigned anVal[3])
{
	CString			strDate;
	unsigned short	anParts[6];
	anParts[0] = (anVal[2] & 0xFFFF0000) >> 16;	// Year
	anParts[1] = (anVal[2] & 0x0000FFFF);			// Mon
	anParts[2] = (anVal[1] & 0xFFFF0000) >> 16;	// Day
	anParts[3] = (anVal[1] & 0x0000FFFF);			// Hour
	anParts[4] = (anVal[0] & 0xFFFF0000) >> 16;	// Min
	anParts[5] = (anVal[0] & 0x0000FFFF);			// Sec
	strDate.Format(_T("%04u-%02u-%02u %02u:%02u:%02u"),
		anParts[0],anParts[1],anParts[2],anParts[3],anParts[4],anParts[5]);
	return strDate;
}