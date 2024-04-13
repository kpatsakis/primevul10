void CjfifDecode::PrepareSignatureThumbSingle(bool bRotate)
{
	CStringA			strTmp;
	CStringA			strSet;

	CStringA			strHashIn;
	unsigned char		pHashIn[2000];
	CStringA			strDqt;
	MD5_CTX				sMd5;
	unsigned			nLenHashIn;
	unsigned			nInd;

	// -----------------------------------------------------------
	// Calculate the MD5 hash for online/internal database!
	// signature "00" : DQT0,DQT1,CSS
	// signature "01" : salt,DQT0,DQT1


	// Build the source string
	// NOTE: For the purposes of the hash, we need to rotate the DQT tables
	// if we detect that the photo is in portrait orientation! This keeps everything
	// consistent.

	// If no DQT tables have been defined (e.g. could have loaded text file!)
	// then override the sig generation!
	bool bDqtDefined = false;
	for (unsigned nSet=0;nSet<4;nSet++) {
		if (m_abImgDqtThumbSet[nSet]) {
			bDqtDefined = true;
		}
	}
	if (!bDqtDefined) {
		m_strHashThumb = _T("NONE");
		m_strHashThumbRot = _T("NONE");
		return;
	}

	if (DB_SIG_VER == 0x00) {
		strHashIn = _T("");
	} else {
		strHashIn = _T("JPEGsnoop");
	}

	//tblSelY = m_anSofQuantTblSel_Tqi[0]; // Y
	//tblSelC = m_anSofQuantTblSel_Tqi[1]; // Cb (should be same as for Cr)

	// Need to duplicate DQT0 if we only have one DQT table

	for (unsigned nSet=0;nSet<4;nSet++) {
		if (m_abImgDqtThumbSet[nSet]) {
			strSet = "";
			strSet.Format("*DQT%u,",nSet);
			strHashIn += strSet;
			for (unsigned i=0;i<64;i++) {
				nInd = (!bRotate)?i:glb_anQuantRotate[i];
				strTmp.Format("%03u,",m_anImgThumbDqt[nSet][nInd]);
				strHashIn += strTmp;
			}
		} // if DQTx defined
	} // loop through sets (DQT0..DQT3)

	// Removed CSS from signature after version 0x00
	if (DB_SIG_VER == 0x00) {
		strHashIn += "*CSS,";
		strHashIn += m_strImgQuantCss;
		strHashIn += ",";
	}
	strHashIn += "*END";
	nLenHashIn = strlen(strHashIn);

	// Display hash input
	for (unsigned i=0;i<nLenHashIn;i+=80) {
		strTmp = "";
		strTmp.Format("In%u: [",i/80);
		strTmp += strHashIn.Mid(i,80);
		strTmp += "]";
#ifdef DEBUG_SIG
		m_pLog->AddLine(strTmp);
#endif
	}

	// Copy into buffer
	ASSERT(nLenHashIn < 2000);
	for (unsigned i=0;i<nLenHashIn;i++) {
		pHashIn[i] = strHashIn.GetAt(i);
	}

	// Calculate the hash
	MD5Init(&sMd5, 0);
	MD5Update(&sMd5, pHashIn, nLenHashIn);
	MD5Final(&sMd5);

	// Overwrite top 8 bits for signature version number
	sMd5.digest32[0] = (sMd5.digest32[0] & 0x00FFFFFF) + (DB_SIG_VER << 24);

	// Convert hash to string format
	if (!bRotate) {
		m_strHashThumb.Format(_T("%08X%08X%08X%08X"),sMd5.digest32[0],sMd5.digest32[1],sMd5.digest32[2],sMd5.digest32[3]);
	} else {
		m_strHashThumbRot.Format(_T("%08X%08X%08X%08X"),sMd5.digest32[0],sMd5.digest32[1],sMd5.digest32[2],sMd5.digest32[3]);
	}

}