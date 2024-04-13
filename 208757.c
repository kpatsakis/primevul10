unsigned CjfifDecode::GetDqtQuantStd(unsigned nInd)
{
	if (nInd < MAX_DQT_COEFF) {
		return glb_anStdQuantLum[nInd];
	} else {
#ifdef DEBUG_LOG
		CString	strTmp;
		CString	strDebug;
		strTmp.Format(_T("GetDqtQuantStd() with nInd out of range. nInd=[%u]"),nInd);
		strDebug.Format(_T("## File=[%-100s] Block=[%-10s] Error=[%s]\n"),(LPCTSTR)m_pAppConfig->strCurFname,
			_T("JfifDecode"),(LPCTSTR)strTmp);
		OutputDebugString(strDebug);
#else
		ASSERT(false);
#endif
		return 0;
	}
}