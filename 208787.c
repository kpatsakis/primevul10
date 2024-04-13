void CjfifDecode::DbgAddLine(LPCTSTR strLine)
{
	if (m_bVerbose)
	{
		m_pLog->AddLine(strLine);
	}
}