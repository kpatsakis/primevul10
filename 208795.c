bool CjfifDecode::ValidateValue(unsigned &nVal,unsigned nMin,unsigned nMax,CString strName,bool bOverride,unsigned nOverrideVal)
{
	CString		strErr;
	if ((nVal >= nMin) && (nVal <= nMax)) {
		// Value is within range
		return true;
	} else {
		if (nVal < nMin) {
			strErr.Format(_T("  ERROR: %s value too small (Actual = %u, Expected >= %u)"),
				(LPCTSTR)strName,nVal,nMin);
			m_pLog->AddLineErr(strErr);
		} else if (nVal > nMax) {
			strErr.Format(_T("  ERROR: %s value too large (Actual = %u, Expected <= %u)"),
				(LPCTSTR)strName,nVal,nMax);
			m_pLog->AddLineErr(strErr);
		}
		if (!m_pAppConfig->bRelaxedParsing) {
			// Defined as fatal error
			// TODO: Replace with glb_strMsgStopDecode?
			m_pLog->AddLineErr(_T("  Stopping decode"));
			m_pLog->AddLineErr(_T("  Use [Relaxed Parsing] to continue"));
			return false;
		} else {
			// Non-fatal
			if (bOverride) {
				// Update value with override
				nVal = nOverrideVal;
				strErr.Format(_T("  WARNING: Forcing value to [%u]"),nOverrideVal);
				m_pLog->AddLineWarn(strErr);
				m_pLog->AddLineWarn(_T("  Resuming decode"));
			} else {
				// No override
				strErr.Format(_T("  Resuming decode"));
				m_pLog->AddLineWarn(strErr);
			}
			return true;
		}
	}

}