void CjfifDecode::DecodeErrCheck(bool bRet)
{
	if (!bRet) {
		if (m_pAppConfig->bInteractive) {
			if (AfxMessageBox(_T("Do you want to continue decoding?"),MB_YESNO|MB_ICONQUESTION)== IDNO) {
				m_bStateAbort = true;
			}
		}
	}
}