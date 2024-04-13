void CjfifDecode::SendSubmit(CString strExifMake, CString strExifModel, CString strQual, 
							CString strDqt0, CString strDqt1, CString strDqt2, CString strDqt3,
							CString strCss,
							CString strSig, CString strSigRot, CString strSigThumb, 
							CString strSigThumbRot, float fQFact0, float fQFact1, unsigned nImgW, unsigned nImgH, 
							CString strExifSoftware, CString strComment, teMaker eMaker,
							teSource eUserSource, CString strUserSoftware, CString strExtra,
							CString strUserNotes, unsigned nExifLandscape,
							unsigned nThumbX,unsigned nThumbY)
{
	// NOTE: This assumes that we've already run PrepareSignature()
	// which usually happens when we process a file.
	ASSERT(strSig != _T(""));
	ASSERT(strSigRot != _T(""));

	CUrlString curls;

	CString DB_SUBMIT_WWW_VER = _T("02");

#ifndef BATCH_DO
	if (m_bSigExactInDB) {
		if (m_pAppConfig->bInteractive)
			AfxMessageBox(_T("Compression signature already in database"));
	} else {

		// Now append it to the local database and resave
		theApp.m_pDbSigs->DatabaseExtraAdd(strExifMake,strExifModel,
			strQual,strSig,strSigRot,strCss,eUserSource,strUserSoftware);

		if (m_pAppConfig->bInteractive)
			AfxMessageBox(_T("Added Compression signature to database"));
	}
#endif


	// Is automatic internet update enabled?
	if (!theApp.m_pAppConfig->bDbSubmitNet) {
		return;
	}

	CString		strTmp;

	CString		strFormat;
	CString		strFormDataPre;
	CString		strFormData;
	unsigned	nFormDataLen;

	unsigned	nChecksum=32;
	CString		strSubmitHost;
	CString		strSubmitPage;
	strSubmitHost = IA_HOST;
	strSubmitPage = IA_DB_SUBMIT_PAGE;
	for (unsigned i=0;i<_tcslen(IA_HOST);i++) {
		nChecksum += strSubmitHost.GetAt(i);
		nChecksum += 3*strSubmitPage.GetAt(i);
	}

	//if ( (m_pAppConfig->bIsWindowsNTorLater) && (nChecksum == 9678) ) {
	if (nChecksum == 9678) {

		// Submit to online database
		CString strHeaders =
			_T("Content-Type: application/x-www-form-urlencoded");
		// URL-encoded form variables -
		strFormat  = _T("ver=%s&x_make=%s&x_model=%s&strUmQual=%s&x_dqt0=%s&x_dqt1=%s&x_dqt2=%s&x_dqt3=%s");
		strFormat += _T("&strXSubsamp=%s&strCSig=%s&strCSigRot=%s&c_qfact0=%f&c_qfact1=%f&x_img_w=%u&x_img_h=%u");
		strFormat += _T("&x_sw=%s&x_com=%s&x_maker=%u&u_source=%d&u_sw=%s");
		strFormat += _T("&x_extra=%s&u_notes=%s&c_sigthumb=%s&c_sigthumbrot=%s&x_landscape=%u");
		strFormat += _T("&x_thumbx=%u&x_thumby=%u");


		strFormDataPre.Format(strFormat,
			DB_SUBMIT_WWW_VER,strExifMake,strExifModel,
			strQual,strDqt0,strDqt1,strDqt2,strDqt3,strCss,strSig,strSigRot,fQFact0,fQFact1,nImgW,nImgH,
			strExifSoftware,strComment,
			eMaker,eUserSource,strUserSoftware,
			strExtra,strUserNotes,
			strSigThumb,strSigThumbRot,nExifLandscape,nThumbX,nThumbY);

		//*** Need to sanitize data for URL submission!
		// Search for "&", "?", "="
		strFormData.Format(strFormat,
			DB_SUBMIT_WWW_VER,curls.Encode(strExifMake),curls.Encode(strExifModel),
			strQual,strDqt0,strDqt1,strDqt2,strDqt3,strCss,strSig,strSigRot,fQFact0,fQFact1,nImgW,nImgH,
			curls.Encode(strExifSoftware),curls.Encode(strComment),
			eMaker,eUserSource,curls.Encode(strUserSoftware),
			curls.Encode(strExtra),curls.Encode(strUserNotes),
			strSigThumb,strSigThumbRot,nExifLandscape,nThumbX,nThumbY);
		nFormDataLen = strFormData.GetLength();


#ifdef DEBUG_SIG
		if (m_pAppConfig->bInteractive) {
			AfxMessageBox(strFormDataPre);
			AfxMessageBox(strFormData);
		}
#endif

#ifdef WWW_WININET
		//static LPSTR astrAcceptTypes[2]={"*/*", NULL};
		HINTERNET hINet, hConnection, hData;

		hINet = InternetOpen(_T("JPEGsnoop/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
		if ( !hINet )
		{
			if (m_pAppConfig->bInteractive)
				AfxMessageBox(_T("InternetOpen Failed"));
			return;
		}
		try
		{
			hConnection = InternetConnect( hINet, (LPCTSTR)strSubmitHost, 80, NULL,NULL, INTERNET_SERVICE_HTTP, 0, 1 );
			if ( !hConnection )
			{
				InternetCloseHandle(hINet);
				return;
			}
			hData = HttpOpenRequest( hConnection, _T("POST"), (LPCTSTR)strSubmitPage, NULL, NULL, NULL, 0, 1 );
			if ( !hData )
			{
				InternetCloseHandle(hConnection);
				InternetCloseHandle(hINet);
				return;
			}
			// GET HttpSendRequest( hData, NULL, 0, NULL, 0);

			HttpSendRequest( hData, (LPCTSTR)strHeaders, strHeaders.GetLength(), strFormData.GetBuffer(), strFormData.GetLength());

		}
		catch( CInternetException* e)
		{
			e->ReportError();
			e->Delete();
			//AfxMessageBox(_T("EXCEPTION!"));
		}
		InternetCloseHandle(hConnection);
		InternetCloseHandle(hINet);
		InternetCloseHandle(hData);
#endif

#ifdef WWW_WINHTTP

		CInternetSession		sSession;
		CHttpConnection*		pConnection;
		CHttpFile*				pFile;
		BOOL					bResult;
		DWORD					dwRet;

		// *** NOTE: Will not work on Windows 95/98!
		// This section is avoided in early OSes otherwise we get an Illegal Op
		try {		
			pConnection = sSession.GetHttpConnection(submit_host);
			ASSERT (pConnection);
			pFile = pConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST,_T(submit_page));
			ASSERT (pFile);
			bResult = pFile->SendRequest(
				strHeaders,(LPVOID)(LPCTSTR)strFormData, strFormData.GetLength());
			ASSERT (bResult != 0);
			pFile->QueryInfoStatusCode(dwRet);
			ASSERT (dwRet == HTTP_STATUS_OK);

			// Clean up!
			if (pFile) {
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}
			if (pConnection) {
				pConnection->Close();
				delete pConnection;
				pConnection = NULL;
			}
			sSession.Close();

		}

		catch (CInternetException* pEx) 
		{
		// catch any exceptions from WinINet      
			TCHAR szErr[MAX_BUF_EX_ERR_MSG];
			szErr[0] = '\0';
			if(!pEx->GetErrorMessage(szErr, MAX_BUF_EX_ERR_MSG))
				_tcscpy(szErr,_T("Unknown error"));
			TRACE("Submit Failed! - %s",szErr);   
			if (m_pAppConfig->bInteractive)
				AfxMessageBox(szErr);
			pEx->Delete();
			if(pFile)
				delete pFile;
			if(pConnection)
				delete pConnection;
			session.Close(); 
			return;
		}
#endif



	}

}