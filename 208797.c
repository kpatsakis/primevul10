bool CjfifDecode::DecodeMakerSubType()
{
	CString strTmp;

	m_nImgExifMakeSubtype = 0;

	if (m_strImgExifMake == _T("NIKON"))
	{
		strTmp = _T("");
		for (unsigned nInd=0;nInd<5;nInd++) {
			strTmp += (char)Buf(m_nPos+nInd);
		}

		if (strTmp == _T("Nikon")) {
			if (Buf(m_nPos+6) == 1) {
				// Type 1
				m_pLog->AddLine(_T("    Nikon Makernote Type 1 detected"));
				m_nImgExifMakeSubtype = 1;
				m_nPos += 8;
			} else if (Buf(m_nPos+6) == 2) {
				// Type 3
				m_pLog->AddLine(_T("    Nikon Makernote Type 3 detected"));
				m_nImgExifMakeSubtype = 3;
				m_nPos += 18;
			} else {
				CString strTmp = _T("ERROR: Unknown Nikon Makernote Type");
				m_pLog->AddLineErr(strTmp);
				if (m_pAppConfig->bInteractive)
					AfxMessageBox(strTmp);
				return FALSE;
			}
		} else {
			// Type 2
			m_pLog->AddLine(_T("    Nikon Makernote Type 2 detected"));
			//m_nImgExifMakeSubtype = 2;
			// tests on D1 seem to indicate that it uses Type 1 headers
			m_nImgExifMakeSubtype = 1;
			m_nPos += 0;
		}

	}
	else if (m_strImgExifMake == _T("SIGMA"))
	{
		strTmp = _T("");
		for (unsigned ind=0;ind<8;ind++) {
			if (Buf(m_nPos+ind) != 0)
				strTmp += (char)Buf(m_nPos+ind);
		}
		if ( (strTmp == _T("SIGMA")) ||
			(strTmp == _T("FOVEON"))  )
		{
			// Valid marker
			// Now skip over the 8-chars and 2 unknown chars
			m_nPos += 10;
		} else {
			CString strTmp = _T("ERROR: Unknown SIGMA Makernote identifier");
			m_pLog->AddLineErr(strTmp);
			if (m_pAppConfig->bInteractive)
				AfxMessageBox(strTmp);
			return FALSE;
		}

	} // SIGMA
	else if (m_strImgExifMake == _T("FUJIFILM"))
	{
		strTmp = _T("");
		for (unsigned ind=0;ind<8;ind++) {
			if (Buf(m_nPos+ind) != 0)
				strTmp += (char)Buf(m_nPos+ind);
		}
		if (strTmp == _T("FUJIFILM"))
		{
			// Valid marker
			// Now skip over the 8-chars and 4 Pointer chars
			// FIXME: Do I need to dereference this pointer?
			m_nPos += 12;
		} else {
			CString strTmp = _T("ERROR: Unknown FUJIFILM Makernote identifier");
			m_pLog->AddLineErr(strTmp);
			if (m_pAppConfig->bInteractive)
				AfxMessageBox(strTmp);
			return FALSE;
		}

	} // FUJIFILM
	else if (m_strImgExifMake == _T("SONY"))
	{
		strTmp = _T("");
		for (unsigned ind=0;ind<12;ind++) {
			if (Buf(m_nPos+ind) != 0)
				strTmp += (char)Buf(m_nPos+ind);
		}
		if (strTmp == _T("SONY DSC "))
		{
			// Valid marker
			// Now skip over the 9-chars and 3 null chars
			m_nPos += 12;
		} else {
			CString strTmp = _T("ERROR: Unknown SONY Makernote identifier");
			m_pLog->AddLineErr(strTmp);
			if (m_pAppConfig->bInteractive)
				AfxMessageBox(strTmp);
			return FALSE;
		}

	} // SONY


	return TRUE;

}