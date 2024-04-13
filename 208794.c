unsigned CjfifDecode::DecodeApp2IccProfile(unsigned nLen)
{
	CString		strTmp;
	unsigned	nMarkerSeqNum;	// Byte
	unsigned	nNumMarkers;	// Byte
	unsigned	nPayloadLen;	// Len of this ICC marker payload

	unsigned	nMarkerPosStart;

	nMarkerSeqNum = Buf(m_nPos++);
	nNumMarkers = Buf(m_nPos++);
	nPayloadLen = nLen - 2 - 12 - 2; // TODO: check?

	strTmp.Format(_T("      Marker Number = %u of %u"),nMarkerSeqNum,nNumMarkers);
	m_pLog->AddLine(strTmp);

	if (nMarkerSeqNum == 1) {
		nMarkerPosStart = m_nPos;
		DecodeIccHeader(nMarkerPosStart);
	} else {
		m_pLog->AddLineWarn(_T("      Only support decode of 1st ICC Marker"));
	}

	return 0;
}