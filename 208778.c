bool CjfifDecode::ExportJpegDoRange(CString strFileIn, CString strFileOut, 
			unsigned long nStart, unsigned long nEnd)
{
	CFile*		pFileOutput;
	CString		strTmp = _T("");

	strTmp.Format(_T("  Exporting range to:   [%s]"),(LPCTSTR)strFileOut);
	m_pLog->AddLine(strTmp);

	if (strFileIn == strFileOut) {
		strTmp.Format(_T("ERROR: Can't overwrite source file. Aborting export."));
		m_pLog->AddLineErr(strTmp);
		if (m_pAppConfig->bInteractive)
			AfxMessageBox(strTmp);

		return false;
	}

	ASSERT(strFileIn != _T(""));
	if (strFileIn == _T("")) {
		strTmp.Format(_T("ERROR: Export but source filename empty"));
		m_pLog->AddLineErr(strTmp);
		if (m_pAppConfig->bInteractive)
			AfxMessageBox(strTmp);

		return false;
	}



	try
	{
		// Open specified file
		// Added in shareDenyNone as this apparently helps resolve some people's troubles
		// with an error showing: Couldn't open file "Sharing Violation"
		pFileOutput = new CFile(strFileOut, CFile::modeCreate| CFile::modeWrite | CFile::typeBinary | CFile::shareDenyNone);
	}
	catch (CFileException* e)
	{
		TCHAR msg[MAX_BUF_EX_ERR_MSG];
		CString strError;
		e->GetErrorMessage(msg,MAX_BUF_EX_ERR_MSG);
		e->Delete();
		strError.Format(_T("ERROR: Couldn't open file for write [%s]: [%s]"),
			(LPCTSTR)strFileOut, (LPCTSTR)msg);
		m_pLog->AddLineErr(strError);
		if (m_pAppConfig->bInteractive)
			AfxMessageBox(strError);
		pFileOutput = NULL;

		return false;

	}


	unsigned		nCopyStart;
	unsigned		nCopyEnd;
	unsigned		nCopyLeft;
	unsigned		ind;

	BYTE*			pBuf;

	pBuf = new BYTE[EXPORT_BUF_SIZE+10];
	if (!pBuf) {
		if (pFileOutput) { delete pFileOutput; pFileOutput = NULL; }
		return false;
	}



	// Step 1
	nCopyStart = nStart;
	nCopyEnd   = nEnd;
	ind = nCopyStart;
	while (ind<nCopyEnd) {
		nCopyLeft = nCopyEnd-ind+1;
		if (nCopyLeft>EXPORT_BUF_SIZE) { nCopyLeft = EXPORT_BUF_SIZE; }
		for (unsigned ind1=0;ind1<nCopyLeft;ind1++) {
			pBuf[ind1] = Buf(ind+ind1,false);
		}
		pFileOutput->Write(pBuf,nCopyLeft);
		ind += nCopyLeft;
		strTmp.Format(_T("Exporting %3u%%..."),ind*100/(nCopyEnd-nCopyStart));
		SetStatusText(strTmp);
	}


	// Free up space
	pFileOutput->Close();

	if (pBuf) {
		delete [] pBuf;
		pBuf = NULL;
	}

	if (pFileOutput) {
		delete pFileOutput;
		pFileOutput = NULL;
	}

	SetStatusText(_T(""));
	strTmp.Format(_T("  Export range done"));
	m_pLog->AddLine(strTmp);

	return true;
}