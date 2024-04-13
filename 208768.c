CString CjfifDecode::PrintAsHex32(unsigned* anWords,unsigned nCount)
{
	CString strVal;
	CString strFull;
	strFull = _T("0x[");
	unsigned nMaxDisplay = MAX_anValues/4;	// Reduce number of words to display since each is 32b not 8b
	bool bExceedMaxDisplay;
	bExceedMaxDisplay = (nCount > nMaxDisplay);
	for (unsigned nInd=0;nInd<nCount;nInd++)
	{
		if (nInd < nMaxDisplay) {
			if (nInd == 0) {
				// Don't do anything for first value!
			} else {
				// Every word add a spacer
				strFull += _T(" ");
			}

			strVal.Format(_T("%08X"),anWords[nInd]);	// 32-bit words
			strFull += strVal;
		}

		if ((nInd == nMaxDisplay) && (bExceedMaxDisplay)) {
			strFull += _T("...");
		}

	}
	strFull += _T("]");
	return strFull;
}