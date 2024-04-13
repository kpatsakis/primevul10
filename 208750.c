bool CjfifDecode::ExportJpegPrepare(CString strFileIn,bool bForceSoi,bool bForceEoi,bool bIgnoreEoi)
{
	// Extract from current file
	//   [m_nPosEmbedStart ... m_nPosEmbedEnd]
	// If state is valid (i.e. file opened)

	CString		strTmp = _T("");

	m_pLog->AddLine(_T(""));
	m_pLog->AddLineHdr(_T("*** Exporting JPEG ***"));

	strTmp.Format(_T("  Exporting from: [%s]"),(LPCTSTR)strFileIn);
	m_pLog->AddLine(strTmp);

	// Only bother to extract if all main sections are present
	bool		bExtractWarn = false;
	CString		strMissing = _T("");

	if (!m_bStateEoi) {
		if (!bForceEoi && !bIgnoreEoi) {
			strTmp.Format(_T("  ERROR: Missing marker: %s"),_T("EOI"));
			m_pLog->AddLineErr(strTmp);
			m_pLog->AddLineErr(_T("         Aborting export. Consider enabling [Force EOI] or [Ignore Missing EOI] option"));
			return false;
		} else if (bIgnoreEoi) {
			// Ignore the EOI, so mark the end of file, but don't
			// set the flag where we force one.
			m_nPosEmbedEnd = m_nPosFileEnd;
		} else {
			// We're missing the EOI but the user has requested
			// that we force an EOI, so let's fix things up
			m_nPosEmbedEnd = m_nPosFileEnd;
		}
	}


	if ((m_nPosEmbedStart == 0) && (m_nPosEmbedEnd == 0)) {
		strTmp.Format(_T("  No frame found at this position in file. Consider using [Img Search]"));
		m_pLog->AddLineErr(strTmp);
		return false;
	}

	if (!m_bStateSoi) {
		if (!bForceSoi) {
			strTmp.Format(_T("  ERROR: Missing marker: %s"),_T("SOI"));
			m_pLog->AddLineErr(strTmp);
			m_pLog->AddLineErr(_T("         Aborting export. Consider enabling [Force SOI] option"));
			return false;
		} else {
			// We're missing the SOI but the user has requested
			// that we force an SOI, so let's fix things up
		}
		
	}
	if (!m_bStateSos) {
		strTmp.Format(_T("  ERROR: Missing marker: %s"),_T("SOS"));
		m_pLog->AddLineErr(strTmp);
		m_pLog->AddLineErr(_T("         Aborting export"));
		return false;
	}

	if (!m_bStateDqt) { bExtractWarn = true; strMissing += _T("DQT "); }
	if (!m_bStateDht) { bExtractWarn = true; strMissing += _T("DHT "); }
	if (!m_bStateSof) { bExtractWarn = true; strMissing += _T("SOF "); }
	
	if (bExtractWarn) {
		strTmp.Format(_T("  NOTE: Missing marker: %s"),(LPCTSTR)strMissing);
		m_pLog->AddLineWarn(strTmp);
		m_pLog->AddLineWarn(_T("        Exported JPEG may not be valid"));
	}

	if (m_nPosEmbedEnd < m_nPosEmbedStart) {
		strTmp.Format(_T("ERROR: Invalid SOI-EOI order. Export aborted."));
		m_pLog->AddLineErr(strTmp);
		if (m_pAppConfig->bInteractive)
			AfxMessageBox(strTmp);
		return false;
	}

	return true;
}