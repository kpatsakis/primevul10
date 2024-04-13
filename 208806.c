void CjfifDecode::DecodeDHT(bool bInject)
{
	unsigned	nLength;
	unsigned	nTmpVal;
	CString		strTmp,strFull;
	unsigned	nPosEnd;
	unsigned	nPosSaved;

	bool		bRet;


	if (bInject) {
		// Redirect Buf() to DHT table in MJPGDHTSeg[]
		// ... so change mode that Buf() call uses
		m_bBufFakeDHT = true;

		// Preserve the "m_nPos" pointer, at end we undo it
		// And we also start at 2 which is just after FFC4 in array
		nPosSaved = m_nPos;
		m_nPos = 2;
	}

	nLength = Buf(m_nPos)*256 + Buf(m_nPos+1);
	nPosEnd = m_nPos+nLength;
	m_nPos+=2;
	strTmp.Format(_T("  Huffman table length = %u"),nLength); 
	m_pLog->AddLine(strTmp);

	unsigned	nDhtClass_Tc;		// Range 0..1
	unsigned	nDhtHuffTblId_Th;	// Range 0..3

	// In various places, added m_bStateAbort check to allow us
	// to escape in case we get in excessive number of DHT entries
	// See BUG FIX #1003

	while ((!m_bStateAbort)&&(nPosEnd > m_nPos))
	{
		m_pLog->AddLine(_T("  ----"));

		nTmpVal = Buf(m_nPos++);
		nDhtClass_Tc = (nTmpVal & 0xF0) >> 4;	// Tc, range 0..1
		nDhtHuffTblId_Th = nTmpVal & 0x0F;		// Th, range 0..3
		strTmp.Format(_T("  Destination ID = %u"),nDhtHuffTblId_Th);
		m_pLog->AddLine(strTmp);
		strTmp.Format(_T("  Class = %u (%s)"),nDhtClass_Tc,(nDhtClass_Tc?_T("AC Table"):_T("DC / Lossless Table")));
		m_pLog->AddLine(strTmp);

		// Add in some error checking to prevent 
		if (nDhtClass_Tc >= MAX_DHT_CLASS) {
			strTmp.Format(_T("ERROR: Invalid DHT Class (%u). Aborting DHT Load."),nDhtClass_Tc);
			m_pLog->AddLineErr(strTmp);
			m_nPos = nPosEnd;
			//m_bStateAbort = true;	// Stop decoding
			break;
		}
		if (nDhtHuffTblId_Th >= MAX_DHT_DEST_ID) {
			strTmp.Format(_T("ERROR: Invalid DHT Dest ID (%u). Aborting DHT Load."),nDhtHuffTblId_Th);
			m_pLog->AddLineErr(strTmp);
			m_nPos = nPosEnd;
			//m_bStateAbort = true;	// Stop decoding
			break;
		}

		// Read in the array of DHT code lengths
		for (unsigned int i=1;i<=MAX_DHT_CODELEN;i++)
		{
			m_anDhtNumCodesLen_Li[i] = Buf(m_nPos++);	// Li, range 0..255
		}

		#define DECODE_DHT_MAX_DHT 256

		unsigned int	anDhtCodeVal[DECODE_DHT_MAX_DHT+1]; // Should only need max 162 codes
		unsigned int	nDhtInd;
		unsigned int	nDhtCodesTotal;

		// Clear out the code list
		for (nDhtInd = 0;nDhtInd <DECODE_DHT_MAX_DHT;nDhtInd++)
		{
			anDhtCodeVal[nDhtInd] = 0xFFFF; // Dummy value
		}

		// Now read in all of the DHT codes according to the lengths
		// read in earlier
		nDhtCodesTotal = 0;
		nDhtInd = 0;
		for (unsigned int nIndLen=1;((!m_bStateAbort)&&(nIndLen<=MAX_DHT_CODELEN));nIndLen++)
		{
			// Keep a total count of the number of DHT codes read
			nDhtCodesTotal += m_anDhtNumCodesLen_Li[nIndLen];

			strFull.Format(_T("    Codes of length %02u bits (%03u total): "),nIndLen,m_anDhtNumCodesLen_Li[nIndLen]);
			for (unsigned int nIndCode=0;((!m_bStateAbort)&&(nIndCode<m_anDhtNumCodesLen_Li[nIndLen]));nIndCode++)
			{
				// Start a new line for every 16 codes
				if ( (nIndCode != 0) && ((nIndCode % 16) == 0) ) {
					strFull = _T("                                         ");
				}
				nTmpVal = Buf(m_nPos++);
				strTmp.Format(_T("%02X "),nTmpVal);
				strFull += strTmp;

				// Only write 16 codes per line
				if ((nIndCode % 16) == 15) {
					m_pLog->AddLine(strFull);
					strFull = _T("");
				}

				// Save the huffman code
				// Just in case we have more DHT codes than we expect, trap
				// the range check here, otherwise we'll have buffer overrun!
				if (nDhtInd < DECODE_DHT_MAX_DHT) {
					anDhtCodeVal[nDhtInd++] = nTmpVal;	// Vij, range 0..255
				} else {
					nDhtInd++;
					strTmp.Format(_T("Excessive DHT entries (%u)... skipping"),nDhtInd);
					m_pLog->AddLineErr(strTmp);
					if (!m_bStateAbort) { DecodeErrCheck(true); }
				}

			}
			m_pLog->AddLine(strFull);
		}
		strTmp.Format(_T("    Total number of codes: %03u"),nDhtCodesTotal);
		m_pLog->AddLine(strTmp);

		unsigned int nDhtLookupInd = 0;

		// Now print out the actual binary strings!
		unsigned long	nBitVal = 0;
		unsigned int	nCodeVal = 0;
		nDhtInd = 0;
		if (m_pAppConfig->bOutputDHTexpand) {
			m_pLog->AddLine(_T(""));
			m_pLog->AddLine(_T("  Expanded Form of Codes:"));
		}
		for (unsigned int nBitLen=1;((!m_bStateAbort)&&(nBitLen<=16));nBitLen++)
		{
			if (m_anDhtNumCodesLen_Li[nBitLen] > 0)
			{
				if (m_pAppConfig->bOutputDHTexpand) {
					strTmp.Format(_T("    Codes of length %02u bits:"),nBitLen);
					m_pLog->AddLine(strTmp);
				}
				// Codes exist for this bit-length
				// Walk through and generate the bitvalues
				for (unsigned int bit_ind=1;((!m_bStateAbort)&&(bit_ind<=m_anDhtNumCodesLen_Li[nBitLen]));bit_ind++)
				{
					unsigned int	nDecVal = nCodeVal;
					unsigned int	nBinBit;
					TCHAR			acBinStr[17] = _T("");
					unsigned int	nBinStrLen = 0;

					// If the user has enabled output of DHT expanded tables,
					// report the bit-string sequences.
					if (m_pAppConfig->bOutputDHTexpand) {
						for (unsigned int nBinInd=nBitLen;nBinInd>=1;nBinInd--)
						{
							nBinBit = (nDecVal >> (nBinInd-1)) & 1;
							acBinStr[nBinStrLen++] = (nBinBit)?'1':'0';
						}
						acBinStr[nBinStrLen] = '\0';
						strFull.Format(_T("      %s = %02X"),acBinStr,anDhtCodeVal[nDhtInd]);

						// The following are only valid for AC components
						// Bug [3442132]
						if (nDhtClass_Tc == DHT_CLASS_AC) {
							if (anDhtCodeVal[nDhtInd] == 0x00) { strFull += _T(" (EOB)"); }
							if (anDhtCodeVal[nDhtInd] == 0xF0) { strFull += _T(" (ZRL)"); }
						}

						strTmp.Format(_T("%-40s (Total Len = %2u)"),(LPCTSTR)strFull,nBitLen + (anDhtCodeVal[nDhtInd] & 0xF));

						m_pLog->AddLine(strTmp);
					}

					// Store the lookup value
					// Shift left to MSB of 32-bit
					unsigned nTmpMask = m_anMaskLookup[nBitLen];

					unsigned nTmpBits = nDecVal << (32-nBitLen);
					unsigned nTmpCode = anDhtCodeVal[nDhtInd];
					bRet = m_pImgDec->SetDhtEntry(nDhtHuffTblId_Th,nDhtClass_Tc,nDhtLookupInd,nBitLen,
						nTmpBits,nTmpMask,nTmpCode);

					DecodeErrCheck(bRet);

					nDhtLookupInd++;

					// Move to the next code
					nCodeVal++;
					nDhtInd++;
				}
			}
			// For each loop iteration (on bit length), we shift the code value
			nCodeVal <<= 1;
		}


		// Now store the dht_lookup_size
		unsigned nTmpSize = nDhtLookupInd;
		bRet = m_pImgDec->SetDhtSize(nDhtHuffTblId_Th,nDhtClass_Tc,nTmpSize);
		if (!m_bStateAbort) { DecodeErrCheck(bRet); }

		m_pLog->AddLine(_T(""));

	}

	if (bInject) {
		// Restore position (as if we didn't move)
		m_nPos = nPosSaved;
		m_bBufFakeDHT = false;
	}
}