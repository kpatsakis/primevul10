CString CjfifDecode::PrintAsHex8(unsigned* anBytes,unsigned nCount)
{
	CString		strVal;
	CString		strFull;
	unsigned	nMaxDisplay = MAX_anValues;
	bool		bExceedMaxDisplay;

	strFull = _T("0x[");
	bExceedMaxDisplay = (nCount > nMaxDisplay);
	for (unsigned nInd=0;nInd<nCount;nInd++)
	{
		if (nInd < nMaxDisplay) {
			if ((nInd % 4) == 0) {
				if (nInd == 0) {
					// Don't do anything for first value!
				} else {
					// Every 16 add big spacer / break
					strFull += _T(" ");
				}
			}

			strVal.Format(_T("%02X"),anBytes[nInd]);
			strFull += strVal;
		}

		if ((nInd == nMaxDisplay) && (bExceedMaxDisplay)) {
			strFull += _T("...");
		}

	}
	strFull += _T("]");
	return strFull;
}