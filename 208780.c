void CjfifDecode::Reset()
{
	// File handling
	m_nPos				= 0;
	m_nPosSos			= 0;
	m_nPosEoi			= 0;
	m_nPosEmbedStart	= 0;
	m_nPosEmbedEnd		= 0;
	m_nPosFileEnd		= 0;

	// SOS / SOF handling
	m_nSofNumLines_Y		= 0;
	m_nSofSampsPerLine_X	= 0;
	m_nSofNumComps_Nf		= 0;

	// Quantization tables
	ClearDQT();

	// Photoshop
	m_nImgQualPhotoshopSfw	= 0;
	m_nImgQualPhotoshopSa	= 0;

	m_nApp14ColTransform = -1;

	// Restart marker
	m_nImgRstEn				= false;
	m_nImgRstInterval		= 0;

	// Basic metadata
	m_strImgExifMake		= _T("???");
	m_nImgExifMakeSubtype	= 0;
	m_strImgExifModel		= _T("???");
	m_bImgExifMakernotes	= false;
	m_strImgExtras			= _T("");
	m_strComment			= _T("");
	m_strSoftware			= _T("");
	m_bImgProgressive		= false;
	m_bImgSofUnsupported	= false;
	_tcscpy_s(m_acApp0Identifier,_T(""));

	// Derived metadata
	m_strHash				= _T("NONE");
	m_strHashRot			= _T("NONE");
	m_eImgLandscape			= ENUM_LANDSCAPE_UNSET;
	m_strImgQualExif		= _T("");
	m_bAvi					= false;
	m_bAviMjpeg				= false;
	m_bPsd					= false;

	// Misc
	m_bImgOK				= false;		// Set during SOF to indicate further proc OK
	m_bBufFakeDHT	   		= false;			// Start in normal Buf mode
	m_eImgEdited			= EDITED_UNSET;
	m_eDbReqSuggest	 		= DB_ADD_SUGGEST_UNSET;
	m_bSigExactInDB	 		= false;

	// Embedded thumbnail
	m_nImgExifThumbComp		= 0;
	m_nImgExifThumbOffset	= 0;
	m_nImgExifThumbLen		= 0;
	m_strHashThumb			= _T("NONE");		// Will go into DB to say NONE!
	m_strHashThumbRot		= _T("NONE");		// Will go into DB to say NONE!
	m_nImgThumbNumLines		= 0;
	m_nImgThumbSampsPerLine	= 0;

	// Now clear out any previously generated bitmaps
	// or image decoding parameters
	if (m_pImgDec) {
		if (m_pImgSrcDirty) {
			m_pImgDec->Reset();
		}
	}

	// Reset the decoding state checks
	// These are to help ensure we don't start decoding SOS
	// if we haven't seen other valid markers yet! Otherwise
	// we could run into very bad loops (e.g. .PSD files)
	// just because we saw FFD8FF first then JFIF_SOS
	m_bStateAbort	= false;
	m_bStateSoi		= false;
	m_bStateDht		= false;
	m_bStateDhtOk	= false;
	m_bStateDhtFake = false;
	m_bStateDqt		= false;
	m_bStateDqtOk	= false;
	m_bStateSof		= false;
	m_bStateSofOk	= false;
	m_bStateSos		= false;
	m_bStateSosOk	= false;
	m_bStateEoi		= false;

}