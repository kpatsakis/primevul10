void CjfifDecode::DecodeEmbeddedThumb()
{
	CString		strTmp;
	CString		strMarker;
	unsigned	nPosSaved;
	unsigned	nPosSaved_sof;
	unsigned	nPosEnd;
	bool		bDone;
	unsigned	nCode;
	bool		bRet;

	CString		strFull;
	unsigned	nDqtPrecision_Pq;
	unsigned	nDqtQuantDestId_Tq;
	unsigned	nImgPrecision;
	unsigned	nLength;
	unsigned	nTmpVal;
	bool		bScanSkipDone;
	bool		bErrorAny = false;
	bool		bErrorThumbLenZero = false;
	unsigned	nSkipCount;

	nPosSaved = m_nPos;

	// Examine the EXIF embedded thumbnail (if it exists)
	if (m_nImgExifThumbComp == 6) {
		m_pLog->AddLine(_T(""));
		m_pLog->AddLineHdr(_T("*** Embedded JPEG Thumbnail ***"));
		strTmp.Format(_T("  Offset: 0x%08X"),m_nImgExifThumbOffset);
		m_pLog->AddLine(strTmp);
		strTmp.Format(_T("  Length: 0x%08X (%u)"),m_nImgExifThumbLen,m_nImgExifThumbLen);
		m_pLog->AddLine(strTmp);

		// Quick scan for DQT tables
		m_nPos = m_nImgExifThumbOffset;
		bDone = false;
		while (!bDone) {

			// For some reason, I have found files that have a nLength of 0
			if (m_nImgExifThumbLen != 0) {
				if ((m_nPos-m_nImgExifThumbOffset) > m_nImgExifThumbLen) {
					strTmp.Format(_T("ERROR: Read more than specified EXIF thumb nLength (%u bytes) before EOI"),m_nImgExifThumbLen);
					m_pLog->AddLineErr(strTmp);
					bErrorAny = true;
					bDone = true;
				}
			} else {
				// Don't try to process if nLength is 0!
				// Seen this in a Canon 1ds file (processed by photoshop)
				bDone = true;
				bErrorAny = true;
				bErrorThumbLenZero = true;
			}
			if ((!bDone) && (Buf(m_nPos++) != 0xFF)) {
				strTmp.Format(_T("ERROR: Expected marker 0xFF, got 0x%02X @ offset 0x%08X"),Buf(m_nPos-1),(m_nPos-1));
				m_pLog->AddLineErr(strTmp);
				bErrorAny = true;
				bDone = true;

			}



			if (!bDone) {
				nCode = Buf(m_nPos++);

 				m_pLog->AddLine(_T(""));
				switch (nCode) {
					case JFIF_SOI: // SOI
						m_pLog->AddLine(_T("  * Embedded Thumb Marker: SOI")); 
						break;

					case JFIF_DQT:  // Define quantization tables
						m_pLog->AddLine(_T("  * Embedded Thumb Marker: DQT")); 

						nLength = Buf(m_nPos)*256 + Buf(m_nPos+1);
						nPosEnd = m_nPos+nLength;
						m_nPos+=2;
						strTmp.Format(_T("    Length = %u"),nLength); 
						m_pLog->AddLine(strTmp);

						while (nPosEnd > m_nPos)
						{
							strTmp.Format(_T("    ----"));
							m_pLog->AddLine(strTmp);

							nTmpVal = Buf(m_nPos++);
							nDqtPrecision_Pq = (nTmpVal & 0xF0) >> 4;
							nDqtQuantDestId_Tq = nTmpVal & 0x0F;
							CString	strPrecision = _T("");
							if (nDqtPrecision_Pq == 0) {
								strPrecision = _T("8 bits");
							} else if (nDqtPrecision_Pq == 1) {
								strPrecision = _T("16 bits");
							} else {
								strPrecision.Format(_T("??? unknown [value=%u]"),nDqtPrecision_Pq);
							}

							strTmp.Format(_T("    Precision=%s"),(LPCTSTR)strPrecision);
							m_pLog->AddLine(strTmp);
							strTmp.Format(_T("    Destination ID=%u"),nDqtQuantDestId_Tq);
							// NOTE: The mapping between destination IDs and the actual
							// usage is defined in the SOF marker which is often later.
							// In nearly all images, the following is true. However, I have
							// seen some test images that set Tbl 3 = Lum, Tbl 0=Chr,
							// Tbl1=Chr, and Tbl2 undefined
							if (nDqtQuantDestId_Tq == 0) {
								strTmp += _T(" (Luminance, typically)");
							}
							else if (nDqtQuantDestId_Tq == 1) {
								strTmp += _T(" (Chrominance, typically)");
							}
							else if (nDqtQuantDestId_Tq == 2) {
								strTmp += _T(" (Chrominance, typically)");
							}
							else {
								strTmp += _T(" (???)");
							}
							m_pLog->AddLine(strTmp);


							if (nDqtQuantDestId_Tq >= 4) {
								strTmp.Format(_T("ERROR: nDqtQuantDestId_Tq = %u, >= 4"),nDqtQuantDestId_Tq);
								m_pLog->AddLineErr(strTmp);
								bDone = true;
								bErrorAny = true;
								break;
							}

							for (unsigned nInd=0;nInd<=63;nInd++)
							{
								nTmpVal = Buf(m_nPos++);
								m_anImgThumbDqt[nDqtQuantDestId_Tq][glb_anZigZag[nInd]] = nTmpVal;
							}
							m_abImgDqtThumbSet[nDqtQuantDestId_Tq] = true;
							
							// Now display the table
							for (unsigned nY=0;nY<8;nY++) {
								strFull.Format(_T("      DQT, Row #%u: "),nY);
								for (unsigned nX=0;nX<8;nX++) {
									strTmp.Format(_T("%3u "),m_anImgThumbDqt[nDqtQuantDestId_Tq][nY*8+nX]);
									strFull += strTmp;

									// Store the DQT entry into the Image DenCoder
									bRet = m_pImgDec->SetDqtEntry(nDqtQuantDestId_Tq,nY*8+nX,
										glb_anUnZigZag[nY*8+nX],m_anImgDqtTbl[nDqtQuantDestId_Tq][nY*8+nX]);
									DecodeErrCheck(bRet);

								}

								m_pLog->AddLine(strFull);

							}
						
						}

						break;

					case JFIF_SOF0:
						m_pLog->AddLine(_T("  * Embedded Thumb Marker: SOF"));
						nLength = Buf(m_nPos)*256 + Buf(m_nPos+1);
						nPosSaved_sof = m_nPos;
						m_nPos+=2;
						strTmp.Format(_T("    Frame header length = %u"),nLength); 
						m_pLog->AddLine(strTmp);


						nImgPrecision = Buf(m_nPos++);
						strTmp.Format(_T("    Precision = %u"),nImgPrecision);
						m_pLog->AddLine(strTmp);

						m_nImgThumbNumLines = Buf(m_nPos)*256 + Buf(m_nPos+1);
						m_nPos += 2;
						strTmp.Format(_T("    Number of Lines = %u"),m_nImgThumbNumLines);
						m_pLog->AddLine(strTmp);

						m_nImgThumbSampsPerLine = Buf(m_nPos)*256 + Buf(m_nPos+1);
						m_nPos += 2; 
						strTmp.Format(_T("    Samples per Line = %u"),m_nImgThumbSampsPerLine);
						m_pLog->AddLine(strTmp);
						strTmp.Format(_T("    Image Size = %u x %u"),m_nImgThumbSampsPerLine,m_nImgThumbNumLines);
						m_pLog->AddLine(strTmp);

						m_nPos = nPosSaved_sof+nLength;

						break;

					case JFIF_SOS: // SOS
						m_pLog->AddLine(_T("  * Embedded Thumb Marker: SOS"));
						m_pLog->AddLine(_T("    Skipping scan data"));
						bScanSkipDone = false;
						nSkipCount = 0;
						while (!bScanSkipDone) {
							if ((Buf(m_nPos) == 0xFF) && (Buf(m_nPos+1) != 0x00)) {
								// Was it a restart marker?
								if ((Buf(m_nPos+1) >= JFIF_RST0) && (Buf(m_nPos+1) <= JFIF_RST7)) {
									m_nPos++;
								} else {
									// No... it's a real marker
									bScanSkipDone = true;
								}
							} else {
								m_nPos++;
								nSkipCount++;
							}
						}
						strTmp.Format(_T("    Skipped %u bytes"),nSkipCount);
						m_pLog->AddLine(strTmp);
						break;

					case JFIF_EOI:
						m_pLog->AddLine(_T("  * Embedded Thumb Marker: EOI")); 
						bDone = true;
						break;
						
					case JFIF_RST0:
					case JFIF_RST1:
					case JFIF_RST2:
					case JFIF_RST3:
					case JFIF_RST4:
					case JFIF_RST5:
					case JFIF_RST6:
					case JFIF_RST7:
						break;
						
					default:
						GetMarkerName(nCode,strMarker);
						strTmp.Format(_T("  * Embedded Thumb Marker: %s"),(LPCTSTR)strMarker); 
						m_pLog->AddLine(strTmp);
						nLength = Buf(m_nPos)*256 + Buf(m_nPos+1);
						strTmp.Format(_T("    Length = %u"),nLength); 
						m_pLog->AddLine(strTmp);
						m_nPos += nLength;
						break;

				}
			} // if !bDone
		} // while !bDone

		// Now calculate the signature
		if (!bErrorAny) {
			PrepareSignatureThumb();
			m_pLog->AddLine(_T(""));
			strTmp.Format(_T("  * Embedded Thumb Signature: %s"),(LPCTSTR)m_strHashThumb);
			m_pLog->AddLine(strTmp);
		}

		if (bErrorThumbLenZero) {
			m_strHashThumb = _T("ERR: Len=0");
			m_strHashThumbRot = _T("ERR: Len=0");
		}

	} // if JPEG compressed

	m_nPos = nPosSaved;
}