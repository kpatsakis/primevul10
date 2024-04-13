void CjfifDecode::OutputSpecial()
{
	CString strTmp;
	CString strFull;

	ASSERT(m_eImgLandscape!=ENUM_LANDSCAPE_UNSET);

	// This mode of operation is currently only used
	// to import the local signature database into a MySQL database
	// backend. It simply reports the MySQL commands which can be input
	// into a MySQL client application.
	if (m_bOutputDB)
	{
		m_pLog->AddLine(_T("*** DB OUTPUT START ***"));
		m_pLog->AddLine(_T("INSERT INTO `quant` (`key`, `make`, `model`, "));
		m_pLog->AddLine(_T("`qual`, `subsamp`, `lum_00`, `lum_01`, `lum_02`, `lum_03`, `lum_04`, "));
		m_pLog->AddLine(_T("`lum_05`, `lum_06`, `lum_07`, `chr_00`, `chr_01`, `chr_02`, "));
		m_pLog->AddLine(_T("`chr_03`, `chr_04`, `chr_05`, `chr_06`, `chr_07`, `qual_lum`, `qual_chr`) VALUES ("));

		strFull = _T("'*KEY*', "); // key -- need to override

		// Might need to change m_strImgExifMake to be lowercase
		strTmp.Format(_T("'%s', "),(LPCTSTR)m_strImgExifMake);
		strFull += strTmp; // make

		strTmp.Format(_T("'%s', "),(LPCTSTR)m_strImgExifModel);
		strFull += strTmp; // model

		strTmp.Format(_T("'%s', "),(LPCTSTR)m_strImgQualExif);
		strFull += strTmp; // quality

		strTmp.Format(_T("'%s', "),(LPCTSTR)m_strImgQuantCss);
		strFull += strTmp; // subsampling

		m_pLog->AddLine(strFull);

		// Step through both quantization tables (0=lum,1=chr)
		unsigned nMatrixInd;

		for (unsigned nDqtInd=0;nDqtInd<2;nDqtInd++) {

			strFull = _T("");
			for (unsigned nY=0;nY<8;nY++) {
				strFull += _T("'");
				for (unsigned nX=0;nX<8;nX++) {
					// Rotate the matrix if necessary!
					nMatrixInd = (m_eImgLandscape!=ENUM_LANDSCAPE_NO)?(nY*8+nX):(nX*8+nY);
					strTmp.Format(_T("%u"),m_anImgDqtTbl[nDqtInd][nMatrixInd]);
					strFull += strTmp;
					if (nX!=7) { strFull += _T(","); }
				}
				strFull += _T("', ");
				if (nY==3) {
					m_pLog->AddLine(strFull);
					strFull = _T("");
				}
			}
			m_pLog->AddLine(strFull);

		}

		strFull = _T("");
		// Output quality ratings
		strTmp.Format(_T("'%f', "),m_adImgDqtQual[0]);
		strFull += strTmp;
		// Don't put out comma separator on last line!
		strTmp.Format(_T("'%f'"),m_adImgDqtQual[1]);
		strFull += strTmp;
		strFull += _T(");");
		m_pLog->AddLine(strFull);

		m_pLog->AddLine(_T("*** DB OUTPUT END ***"));
	}

}