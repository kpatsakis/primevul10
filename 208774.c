bool CjfifDecode::DecodeValGPS(unsigned nPos,CString &strCoord)
{
	float		fCoord1,fCoord2,fCoord3;
	bool		bRet;

	bRet = true;
	if (bRet) { bRet = DecodeValRational(nPos,fCoord1); nPos += 8; }
	if (bRet) { bRet = DecodeValRational(nPos,fCoord2); nPos += 8; }
	if (bRet) { bRet = DecodeValRational(nPos,fCoord3); nPos += 8; }

	if (!bRet) {
		strCoord.Format(_T("???"));
		return false;
	} else {
		return PrintValGPS(3,fCoord1,fCoord2,fCoord3,strCoord);
	}
}