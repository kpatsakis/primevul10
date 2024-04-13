unsigned CjfifDecode::DecodeApp13Ps()
{
	// Photoshop APP13 marker definition doesn't appear to have a 
	// well-defined length, so I will read until I encounter a 
	// non-"8BIM" entry, then we reset the position counter
	// and move on to the next marker.
	// FIXME: This does not appear to be very robust

	CString			strTmp;
	CString			strBimName;
	bool			bDone = false;

	unsigned		nVal = 0x8000;
	unsigned		nSaveFormat = 0;

	CString			strVal;
	CString			strByte;

	CString			strBimSig;

	// Reset PsDec decoder state
	m_pPsDec->Reset();

	while (!bDone)
	{
		// FIXME: Need to check for actual marker section extent, not
		// just the lack of an 8BIM signature as the terminator!

		// Check the signature but don't advance the file pointer
		strBimSig = m_pWBuf->BufReadStrn(m_nPos,4);
		// Check for signature "8BIM"
		if (strBimSig == _T("8BIM")) {
			m_pPsDec->PhotoshopParseImageResourceBlock(m_nPos,3);
		} else {
			// Not 8BIM?
			bDone = true;
		}
	}

	// Now that we've finished with the PsDec decoder we can fetch
	// some of the parser state
	// TODO: Migrate into accessor
	m_nImgQualPhotoshopSa = m_pPsDec->m_nQualitySaveAs;
	m_nImgQualPhotoshopSfw = m_pPsDec->m_nQualitySaveForWeb;
	m_bPsd = m_pPsDec->m_bPsd;

	return 0;
}