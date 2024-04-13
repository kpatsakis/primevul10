bool CjfifDecode::ExpectMarkerEnd(unsigned long nMarkerStart,unsigned nMarkerLen)
{
	CString			strTmp;
	unsigned long	nMarkerEnd = nMarkerStart + nMarkerLen;
	unsigned long	nMarkerExtra = nMarkerEnd - m_nPos;
	if (m_nPos < nMarkerEnd) {
		// The length indicates that there is more data than we processed
		strTmp.Format(_T("  WARNING: Marker length longer than expected"));
		m_pLog->AddLineWarn(strTmp);
		if (!m_pAppConfig->bRelaxedParsing) {
			// Abort
			m_pLog->AddLineErr(_T("  Stopping decode"));
			m_pLog->AddLineErr(_T("  Use [Img Search Fwd/Rev] to locate other valid embedded JPEGs"));
			return false;
		} else {
			// Warn and skip
			strTmp.Format(_T("  Skipping remainder [%u bytes]"),nMarkerExtra);
			m_pLog->AddLineWarn(strTmp);
			m_nPos += nMarkerExtra;
		}
	} else if (m_nPos > nMarkerEnd) {
		// The length indicates that there is less data than we processed
		strTmp.Format(_T("  WARNING: Marker length shorter than expected"));
		m_pLog->AddLineWarn(strTmp);
		if (!m_pAppConfig->bRelaxedParsing) {
			// Abort
			m_pLog->AddLineErr(_T("  Stopping decode"));
			m_pLog->AddLineErr(_T("  Use [Img Search Fwd/Rev] to locate other valid embedded JPEGs"));
			return false;
		} else {
			// Warn but no skip
			// Note that we can't skip as the length would imply a rollback
			// Most resilient solution is probably to assume length was
			// wrong and continue from where the marker should have ended.
			// For resiliency, attempt two methods to find point to resume:
			// 1) Current position
			// 2) Actual length defined in marker
			if (Buf(m_nPos) == 0xFF) {
				// Using actual data expected seems more promising
				m_pLog->AddLineWarn(_T("  Resuming decode"));
			} else if (Buf(nMarkerEnd) == 0xFF) {
				// Using actual length seems more promising
				m_nPos = nMarkerEnd;
				m_pLog->AddLineWarn(_T("  Rolling back pointer to end indicated by length"));
				m_pLog->AddLineWarn(_T("  Resuming decode"));
			} else {
				// No luck. Expect marker failure now
				m_pLog->AddLineWarn(_T("  Resuming decode"));
			}
		}
	}

	// If we get here, then we haven't seen a fatal issue
	return true;
}