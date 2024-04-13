void CjfifDecode::SetDQTQuick(unsigned anDqt0[64],unsigned anDqt1[64])
{
	m_eImgLandscape = ENUM_LANDSCAPE_YES;
	for (unsigned ind=0;ind<MAX_DQT_COEFF;ind++)
	{
		m_anImgDqtTbl[0][ind] = anDqt0[ind];
		m_anImgDqtTbl[1][ind] = anDqt1[ind];
	}
	m_abImgDqtSet[0] = true;
	m_abImgDqtSet[1] = true;
	m_strImgQuantCss = _T("NA");
}