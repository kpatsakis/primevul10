bool CjfifDecode::DecodeAvi()
{
	CString		strTmp;
	unsigned	nPosSaved;

	m_bAvi = false;
	m_bAviMjpeg = false;

	// Perhaps start from file position 0?
	nPosSaved = m_nPos;

	// Start from file position 0
	m_nPos = 0;

	bool		bSwap = true;

	CString		strRiff;
	unsigned	nRiffLen;
	CString		strForm;

	strRiff = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
	nRiffLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
	strForm = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
	if ((strRiff == _T("RIFF")) && (strForm == _T("AVI "))) {
		m_bAvi = true;
		m_pLog->AddLine(_T(""));
		m_pLog->AddLineHdr(_T("*** AVI File Decoding ***"));
		m_pLog->AddLine(_T("Decoding RIFF AVI format..."));
		m_pLog->AddLine(_T(""));
	} else {
		// Reset file position
		m_nPos = nPosSaved;
		return false;
	}

	CString		strHeader;
	unsigned	nChunkSize;
	unsigned	nChunkDataStart;

	bool	done = false;
	while (!done) {
		if (m_nPos >= m_pWBuf->GetPosEof()) {
			done = true;
			break;
		}

		strHeader = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
        strTmp.Format(_T("  %s"),(LPCTSTR)strHeader);
		m_pLog->AddLine(strTmp);

		nChunkSize = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
		nChunkDataStart = m_nPos;


		if (strHeader == _T("LIST")) {

			// --- LIST ---

			CString strListType;
			strListType = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;

			strTmp.Format(_T("    %s"),(LPCTSTR)strListType);
			m_pLog->AddLine(strTmp);

			if (strListType == _T("hdrl")) {

				// --- hdrl ---

				unsigned nPosHdrlStart;
				CString strHdrlId;
				strHdrlId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
				unsigned nHdrlLen;
				nHdrlLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				nPosHdrlStart = m_nPos;

				// nHdrlLen should be 14*4 bytes

				m_nPos = nPosHdrlStart + nHdrlLen;

			} else if (strListType == _T("strl")) {

				// --- strl ---

				// strhHEADER
				unsigned nPosStrlStart;
				CString strStrlId;
				strStrlId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
				unsigned nStrhLen;
				nStrhLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				nPosStrlStart = m_nPos;

				CString fccType;
				CString fccHandler;
				unsigned dwFlags,dwReserved1,dwInitialFrames,dwScale,dwRate;
				unsigned dwStart,dwLength,dwSuggestedBufferSize,dwQuality;
				unsigned dwSampleSize,xdwQuality,xdwSampleSize;
				fccType = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
				fccHandler = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
				dwFlags = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwReserved1 = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwInitialFrames = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwScale = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwRate = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwStart = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwLength = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwSuggestedBufferSize = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwQuality = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				dwSampleSize = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				xdwQuality = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				xdwSampleSize = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;

				CString fccTypeDecode = _T("");
				if (fccType == _T("vids")) { fccTypeDecode = _T("[vids] Video"); }
				else if (fccType == _T("auds")) { fccTypeDecode = _T("[auds] Audio"); }
				else if (fccType == _T("txts")) { fccTypeDecode = _T("[txts] Subtitle"); }
				else { fccTypeDecode.Format(_T("[%s]"),(LPCTSTR)fccType); } 
				strTmp.Format(_T("      -[FourCC Type]  = %s"),(LPCTSTR)fccTypeDecode);
				m_pLog->AddLine(strTmp);

				strTmp.Format(_T("      -[FourCC Codec] = [%s]"),(LPCTSTR)fccHandler);
				m_pLog->AddLine(strTmp);

				float fSampleRate = 0;
				if (dwScale != 0) {
					fSampleRate = (float)dwRate / (float)dwScale;
				}
				strTmp.Format(_T("      -[Sample Rate]  = [%.2f]"),fSampleRate);
				if (fccType == _T("vids")) { strTmp.Append(_T(" frames/sec")); }
				else if (fccType == _T("auds")) { strTmp.Append(_T(" samples/sec")); }
				m_pLog->AddLine(strTmp);

				m_nPos = nPosStrlStart + nStrhLen;	// Skip

				strTmp.Format(_T("      %s"),(LPCTSTR)fccType);
				m_pLog->AddLine(strTmp);

				if (fccType == _T("vids")) {
					// --- vids ---

					// Is it MJPEG?
					//strTmp.Format(_T("      -[Video Stream FourCC]=[%s]"),fccHandler);
					//m_pLog->AddLine(strTmp);
					if (fccHandler == _T("mjpg")) {
						m_bAviMjpeg = true;
					}
					if (fccHandler == _T("MJPG")) {
						m_bAviMjpeg = true;
					}

					// strfHEADER_BIH
					CString strSkipId;
					unsigned nSkipLen;
					unsigned nSkipStart;
					strSkipId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
					nSkipLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
					nSkipStart = m_nPos;
					
					m_nPos = nSkipStart + nSkipLen; // Skip

				} else if (fccType == _T("auds")) {
					// --- auds ---

					// strfHEADER_WAVE

					CString strSkipId;
					unsigned nSkipLen;
					unsigned nSkipStart;
					strSkipId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
					nSkipLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
					nSkipStart = m_nPos;
					
					m_nPos = nSkipStart + nSkipLen; // Skip
				} else {
					// strfHEADER

					CString strSkipId;
					unsigned nSkipLen;
					unsigned nSkipStart;
					strSkipId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
					nSkipLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
					nSkipStart = m_nPos;
					
					m_nPos = nSkipStart + nSkipLen; // Skip
				}

				// strnHEADER
				unsigned nPosStrnStart;
				CString strStrnId;
				strStrnId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
				unsigned nStrnLen;
				nStrnLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;
				nPosStrnStart = m_nPos;

				// FIXME: Can we rewrite in terms of ChunkSize and ChunkDataStart?
				//ASSERT ((nPosStrnStart + nStrnLen + (nStrnLen%2)) == (nChunkDataStart + nChunkSize + (nChunkSize%2)));
				//m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);
				m_nPos = nPosStrnStart + nStrnLen + (nStrnLen%2); // Skip

			} else if (strListType == _T("movi")) {
				// movi

				m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);
			} else if (strListType == _T("INFO")) {
				// INFO
				unsigned nInfoStart;
				nInfoStart = m_nPos;

				CString strInfoId;
				unsigned nInfoLen;
				strInfoId = m_pWBuf->BufReadStrn(m_nPos,4); m_nPos+=4;
				nInfoLen = m_pWBuf->BufX(m_nPos,4,bSwap); m_nPos+=4;

				if (strInfoId == _T("ISFT")) {
					CString strIsft=_T("");
					strIsft = m_pWBuf->BufReadStrn(m_nPos,nChunkSize);
					strIsft.TrimRight();
					strTmp.Format(_T("      -[Software] = [%s]"),(LPCTSTR)strIsft);
					m_pLog->AddLine(strTmp);
				}

				m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);
			} else {
				// ?
				m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);
			}
			
		} else if (strHeader == _T("JUNK")) {
			// Junk

			m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);
		} else if (strHeader == _T("IDIT")) {
			// Timestamp info (Canon, etc.)

			CString strIditTimestamp=_T("");
			strIditTimestamp = m_pWBuf->BufReadStrn(m_nPos,nChunkSize);
			strIditTimestamp.TrimRight();
			strTmp.Format(_T("    -[Timestamp] = [%s]"),(LPCTSTR)strIditTimestamp);
			m_pLog->AddLine(strTmp);
		
			m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);

		} else if (strHeader == _T("indx")) {
			// Index

			m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);
		} else if (strHeader == _T("idx1")) {
			// Index
			unsigned nIdx1Entries = nChunkSize / (4*4);

			m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);

		} else {
			// Unsupported
			m_nPos = nChunkDataStart + nChunkSize + (nChunkSize%2);

		}

	}

	m_pLog->AddLine(_T(""));
	if (m_bAviMjpeg) {
		m_strImgExtras += _T("[AVI]:[mjpg],");
		m_pLog->AddLineGood(_T("  AVI is MotionJPEG"));
		m_pLog->AddLineWarn(_T("  Use [Tools->Img Search Fwd] to locate next frame"));
	} else {
		m_strImgExtras += _T("[AVI]:[????],");
		m_pLog->AddLineWarn(_T("  AVI is not MotionJPEG. [Img Search Fwd/Rev] unlikely to find frames."));
	}
	m_pLog->AddLine(_T(""));

	// Reset file position
	m_nPos = nPosSaved;

	return m_bAviMjpeg;
}