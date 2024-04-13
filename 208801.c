void CjfifDecode::ProcessFile(CFile* inFile)
{

	CString strTmp;

	// Reset the JFIF decoder state as we may be redoing another file
	Reset();

	// Reset the IMG Decoder state
	if (m_pImgSrcDirty) {
		m_pImgDec->ResetState();
	}

	// Set the statusbar text to Processing...

	// Ensure the status bar has been allocated
	// NOTE: The stat bar is NULL if we drag & drop a file onto
	//       the JPEGsnoop app icon.
	if (m_pStatBar) {
		m_pStatBar->SetPaneText(0,_T("Processing..."));
	}


	// Note that we don't clear out the logger (with m_pLog->Reset())
	// as we want top-level caller to do this. This way we can
	// still insert extra lines from top level.

	// GetLength returns ULONGLONG. Abort on large files (>=4GB)
	ULONGLONG	nPosFileEnd;
	nPosFileEnd = inFile->GetLength();
	if (nPosFileEnd > 0xFFFFFFFFUL) {
		CString strTmp = _T("File too large. Skipping.");
		m_pLog->AddLineErr(strTmp);
		if (m_pAppConfig->bInteractive)
			AfxMessageBox(strTmp);
		return;
	}
	m_nPosFileEnd = static_cast<unsigned long>(nPosFileEnd);


	unsigned nStartPos;
	nStartPos = m_pAppConfig->nPosStart;
	m_nPos = nStartPos;
	m_nPosEmbedStart = nStartPos;	// Save the embedded file start position

	strTmp.Format(_T("Start Offset: 0x%08X"),nStartPos);
	m_pLog->AddLine(strTmp);


	// ----------------------------------------------------------------

	// Test for AVI file
	// - Detect header
	// - start from beginning of file
	DecodeAvi();
	// TODO: Should we skip decode of file if not MJPEG?


	// ----------------------------------------------------------------

	// Test for PSD file
	// - Detect header
	// - FIXME: start from current offset?
	unsigned	nWidth=0;
	unsigned	nHeight=0;

#ifdef PS_IMG_DEC_EN
	// If PSD image decoding is enabled, associate the PSD parsing with
	// the current DIB. After decoding, flag the DIB as ready for display.

	// Attempt decode as PSD
	bool bDecPsdOk;
	bDecPsdOk = m_pPsDec->DecodePsd(nStartPos,&m_pImgDec->m_pDibTemp,nWidth,nHeight);
	if (bDecPsdOk) {
		// FIXME: The following is a bit of a hack
		m_pImgDec->m_bDibTempReady = true;
		m_pImgDec->m_bPreviewIsJpeg = false;			// MCU/Block info not available
		m_pImgDec->SetImageDimensions(nWidth,nHeight);

		// Clear the image information
		// The primary reason for this is to ensure we don't have stale information from a previous
		// JPEG image (eg. real image border inside MCU border which would be overlayed during draw).
		m_pImgDec->SetImageDetails(0,0,0,0,false,0);
		
		// No more processing of file
		// - Otherwise we'd continue to attempt to decode as JPEG
		return;
	}
#else
	// Don't attempt to display Photoshop image data
	if (m_pPsDec->DecodePsd(nStartPos,NULL,nWidth,nHeight)) {
		return;
	}
#endif


	// ----------------------------------------------------------------

// Disable DICOM for now until fully tested
#ifdef SUPPORT_DICOM
	// Test for DICOM
	// - Detect header
	// - start from beginning of file
	bool			bDicom = false;
	unsigned long	nPosJpeg = 0;		// File offset to embedded JPEG in DICOM
	bDicom = m_pDecDicom->DecodeDicom(0,m_nPosFileEnd,nPosJpeg);
	if (bDicom) {
		// Adjust start of JPEG decoding if we are currently without an offset
		if (nStartPos == 0) {
			m_pAppConfig->nPosStart = nPosJpeg;

			nStartPos = m_pAppConfig->nPosStart;
			m_nPos = nStartPos;
			m_nPosEmbedStart = nStartPos;	// Save the embedded file start position

			strTmp.Format(_T("Adjusting Start Offset to: 0x%08X"),nStartPos);
			m_pLog->AddLine(strTmp);
			m_pLog->AddLine(_T(""));
		}
	}
#endif


	// ----------------------------------------------------------------

	// Decode as JPEG JFIF file

	// If we are in a non-zero offset, add this to extras
	if (m_pAppConfig->nPosStart!=0) {
		strTmp.Format(_T("[Offset]=[%lu],"),m_pAppConfig->nPosStart);
        m_strImgExtras += strTmp;
	}

	unsigned nDataAfterEof = 0;

	BOOL bDone = FALSE;
	while (!bDone)
	{
		// Allow some other threads to jump in

		// Return value 0 - OK
		//              1 - Error
		//              2 - EOI
		if (DecodeMarker() != DECMARK_OK) {
			bDone = TRUE;
			if (m_nPosFileEnd >= m_nPosEoi) {
				nDataAfterEof = m_nPosFileEnd - m_nPosEoi;
			}
		} else {
			if (m_nPos > m_pWBuf->GetPosEof()) {
				m_pLog->AddLineErr(_T("ERROR: Early EOF - file may be missing EOI"));
				bDone = TRUE;
			}
		}
	}

	// -----------------------------------------------------------
	// Perform any other informational calculations that require all tables
	// to be present.

	// Determine the CSS Ratio
	// Save the subsampling string. Assume component 2 is representative of the overall chrominance.

	// NOTE: Ensure that we don't execute the following code if we haven't
	//       completed our read (ie. get bad marker earlier in processing).
	// TODO: What is the best way to determine all is OK?

	m_strImgQuantCss = _T("?x?");
	m_strHash = _T("NONE");
	m_strHashRot = _T("NONE");

	if (m_bImgOK) {
		ASSERT(m_eImgLandscape!=ENUM_LANDSCAPE_UNSET);

		if (m_nSofNumComps_Nf == NUM_CHAN_YCC) {
			// We only try to determine the chroma subsampling ratio if we have 3 components (assume YCC)
			// In general, we should be able to use the 2nd or 3rd component
		
			// NOTE: The following assumes m_anSofHorzSampFact_Hi and m_anSofVertSampFact_Vi
			// are non-zero as otherwise we'll have a divide-by-0 exception.
			unsigned	nCompIdent = m_anSofQuantCompId[SCAN_COMP_CB];
			unsigned	nCssFactH = m_nSofHorzSampFactMax_Hmax/m_anSofHorzSampFact_Hi[nCompIdent];
			unsigned	nCssFactV = m_nSofVertSampFactMax_Vmax/m_anSofVertSampFact_Vi[nCompIdent];
			if (m_eImgLandscape!=ENUM_LANDSCAPE_NO) {
				// Landscape orientation
				m_strImgQuantCss.Format(_T("%ux%u"),nCssFactH,nCssFactV);
			}
			else {
				// Portrait orientation (flip subsampling ratio)
				m_strImgQuantCss.Format(_T("%ux%u"),nCssFactV,nCssFactH);
			}
		} else if (m_nSofNumComps_Nf == NUM_CHAN_GRAYSCALE) {
			m_strImgQuantCss = _T("Gray");
		}

		DecodeEmbeddedThumb();

		// Generate the signature
		PrepareSignature();

		// Compare compression signature
		if (m_pAppConfig->bSigSearch) {
			// In the case of lossless files, there won't be any DQT and
			// hence no compression signatures to compare. Therefore, skip this process.
			if (m_strHash == _T("NONE")) {
				m_pLog->AddLineWarn(_T("Skipping compression signature search as no DQT"));
			} else {
				CompareSignature();
			}
		}

		if (nDataAfterEof > 0) {
			m_pLog->AddLine(_T(""));
			m_pLog->AddLineHdr(_T("*** Additional Info ***"));
			strTmp.Format(_T("NOTE: Data exists after EOF, range: 0x%08X-0x%08X (%u bytes)"),
				m_nPosEoi,m_nPosFileEnd,nDataAfterEof);
			m_pLog->AddLine(strTmp);
		}

		// Print out the special-purpose outputs
		OutputSpecial();
	}


	// Reset the status bar text
	if (m_pStatBar) {
		m_pStatBar->SetPaneText(0,_T("Done"));
	}

	// Mark the file as closed
	//m_pWBuf->BufFileUnset();

}