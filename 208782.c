bool CjfifDecode::GetMarkerName(unsigned nCode,CString &markerStr)
{
	bool		bDone = false;
	bool		bFound = false;
	unsigned	nInd=0;

	while (!bDone)
	{
		if (m_pMarkerNames[nInd].nCode==0) {
			bDone = true;
		} else if (m_pMarkerNames[nInd].nCode==nCode) {
			bDone = true;
			bFound = true;
			markerStr = m_pMarkerNames[nInd].strName;
			return true;
		} else {
			nInd++;
		}
	}
	if (!bFound) {
		markerStr = _T("");
		markerStr.Format(_T("(0xFF%02X)"),nCode);
		return false;
	}
	return true;

}