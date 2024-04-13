void CjfifDecode::ClearDQT()
{
	for (unsigned nTblInd=0;nTblInd<MAX_DQT_DEST_ID;nTblInd++)
	{
		for (unsigned nCoeffInd=0;nCoeffInd<MAX_DQT_COEFF;nCoeffInd++)
		{
			m_anImgDqtTbl[nTblInd][nCoeffInd] = 0;
			m_anImgThumbDqt[nTblInd][nCoeffInd] = 0;
		}
		m_adImgDqtQual[nTblInd] = 0;
		m_abImgDqtSet[nTblInd] = false;
		m_abImgDqtThumbSet[nTblInd] = false;
	}
}