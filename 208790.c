void CjfifDecode::PrepareSendSubmit(CString strQual,teSource eUserSource,CString strUserSoftware,CString strUserNotes)
{
	// Generate the DQT arrays suitable for posting
	CString		strTmp1;
	CString		asDqt[4];
	unsigned	nMatrixInd;

	ASSERT(m_strHash != _T("NONE"));
	ASSERT(m_eImgLandscape!=ENUM_LANDSCAPE_UNSET);

	for (unsigned nSet=0;nSet<4;nSet++) {
		asDqt[nSet] = _T("");

		if (m_abImgDqtSet[nSet]) {
			for (unsigned nInd=0;nInd<64;nInd++) {
				// FIXME: Still consider rotating DQT table even though we
				// don't know for sure if m_eImgLandscape is accurate
				// Not a big deal if we get it wrong as we still add
				// both pre- and post-rotated sigs.
				nMatrixInd = (m_eImgLandscape!=ENUM_LANDSCAPE_NO)?nInd:glb_anQuantRotate[nInd];
				if ((nInd%8 == 0) && (nInd != 0)) {
					asDqt[nSet].Append(_T("!"));
				}

				asDqt[nSet].AppendFormat(_T("%u"),m_anImgDqtTbl[nSet][nMatrixInd]);
				if (nInd%8 != 7) {
					asDqt[nSet].Append(_T(","));
				}
			}
		} // set defined?
	} // up to 4 sets

	unsigned nOrigW,nOrigH;
	nOrigW = (m_eImgLandscape!=ENUM_LANDSCAPE_NO)?m_nSofSampsPerLine_X:m_nSofNumLines_Y;
	nOrigH = (m_eImgLandscape!=ENUM_LANDSCAPE_NO)?m_nSofNumLines_Y:m_nSofSampsPerLine_X;

	unsigned nOrigThumbW,nOrigThumbH;
	nOrigThumbW = (m_eImgLandscape!=ENUM_LANDSCAPE_NO)?m_nImgThumbSampsPerLine:m_nImgThumbNumLines;
	nOrigThumbH = (m_eImgLandscape!=ENUM_LANDSCAPE_NO)?m_nImgThumbNumLines:m_nImgThumbSampsPerLine;

	teMaker eMaker;
	eMaker = (m_bImgExifMakernotes)?ENUM_MAKER_PRESENT:ENUM_MAKER_NONE;

	// Sort sig additions
	// To create some determinism in the database, arrange the sigs
	// to be in numerical order
	CString strSig0,strSig1,strSigThm0,strSigThm1;
	if (m_strHash <= m_strHashRot) {
		strSig0 = m_strHash;
		strSig1 = m_strHashRot;
	} else {
		strSig0 = m_strHashRot;
		strSig1 = m_strHash;
	}
	if (m_strHashThumb <= m_strHashThumbRot) {
		strSigThm0 = m_strHashThumb;
		strSigThm1 = m_strHashThumbRot;
	} else {
		strSigThm0 = m_strHashThumbRot;
		strSigThm1 = m_strHashThumb;
	}

	SendSubmit(m_strImgExifMake,m_strImgExifModel,strQual,asDqt[0],asDqt[1],asDqt[2],asDqt[3],m_strImgQuantCss,
		strSig0,strSig1,strSigThm0,strSigThm1,(float)m_adImgDqtQual[0],(float)m_adImgDqtQual[1],nOrigW,nOrigH,
		m_strSoftware,m_strComment,eMaker,eUserSource,strUserSoftware,m_strImgExtras,
		strUserNotes,m_eImgLandscape,nOrigThumbW,nOrigThumbH);

}