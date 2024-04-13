CjfifDecode::CjfifDecode(CDocLog* pLog,CwindowBuf* pWBuf,CimgDecode* pImgDec)
{
	// Ideally this would be passed by constructor, but simply access
	// directly for now.
	CJPEGsnoopApp*	pApp;
	pApp = (CJPEGsnoopApp*)AfxGetApp();
    m_pAppConfig = pApp->m_pAppConfig;
	ASSERT(m_pAppConfig);
	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() Begin"));

	ASSERT(pLog);
	ASSERT(pWBuf);
	ASSERT(pImgDec);

	// Need to zero out the private members
	m_bOutputDB = FALSE;		// mySQL output for web

	// Enable verbose reporting
	m_bVerbose = FALSE;

	m_pImgSrcDirty = TRUE;

	// Generate lookup tables for Huffman codes
	GenLookupHuffMask();
	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() Checkpoint 1"));

	// Window status bar is not ready yet, wait for call to SetStatusBar()
	m_pStatBar = NULL;

	// Save copies of other class pointers
	m_pLog = pLog;
	m_pWBuf = pWBuf;
	m_pImgDec = pImgDec;

	// Reset decoding state
	Reset();
	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() Checkpoint 2"));

	// Load the local database (if it exists)
	theApp.m_pDbSigs->DatabaseExtraLoad();
	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() Checkpoint 3"));

	// Allocate the Photoshop decoder
	m_pPsDec = new CDecodePs(pWBuf,pLog);
	if (!m_pPsDec) {
		ASSERT(false);
		return;
	}
	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() Checkpoint 4"));

#ifdef SUPPORT_DICOM
	// Allocate the DICOM decoder
	m_pDecDicom = new CDecodeDicom(pWBuf,pLog);
	if (!m_pDecDicom) {
		ASSERT(false);
		return;
	}
	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() Checkpoint 5"));
#endif

	if (DEBUG_EN) m_pAppConfig->DebugLogAdd(_T("CjfifDecode::CjfifDecode() End"));
}