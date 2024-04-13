unsigned CjfifDecode::DecodeApp2Flashpix()
{

	CString strTmp;

	unsigned	nFpxVer;
	unsigned	nFpxSegType;
	unsigned	nFpxInteropCnt;
	unsigned	nFpxEntitySz;
	unsigned	nFpxDefault;

	bool		bFpxStorage;
	CString		strFpxStorageClsStr;
	unsigned	nFpxStIndexCont;
	unsigned	nFpxStOffset;

	unsigned	nFpxStWByteOrder;
	unsigned	nFpxStWFormat;
	CString		strFpxStClsidStr;
	unsigned	nFpxStDwOsVer;
	unsigned	nFpxStRsvd;

	CString		streamStr;

	nFpxVer = Buf(m_nPos++);
	nFpxSegType = Buf(m_nPos++);

	// FlashPix segments: Contents List or Stream Data

	if (nFpxSegType == 1) {
		// Contents List
		strTmp.Format(_T("    Segment: CONTENTS LIST"));
		m_pLog->AddLine(strTmp);

		nFpxInteropCnt = (Buf(m_nPos++)<<8) + Buf(m_nPos++);
		strTmp.Format(_T("      Interoperability Count = %u"),nFpxInteropCnt);
		m_pLog->AddLine(strTmp);

		for (unsigned ind=0;ind<nFpxInteropCnt;ind++) {
			strTmp.Format(_T("      Entity Index #%u"),ind); 
			m_pLog->AddLine(strTmp);
			nFpxEntitySz = (Buf(m_nPos++)<<24) + (Buf(m_nPos++)<<16) + (Buf(m_nPos++)<<8) + Buf(m_nPos++);

			// If the "entity size" field is 0xFFFFFFFF, then it should be treated as
			// a "storage". It looks like we should probably be using this to determine
			// that we have a "storage"
			bFpxStorage = false;
			if (nFpxEntitySz == 0xFFFFFFFF) {
				bFpxStorage = true;
			}

			if (!bFpxStorage) {
				strTmp.Format(_T("        Entity Size = %u"),nFpxEntitySz);
				m_pLog->AddLine(strTmp);
			} else {
				strTmp.Format(_T("        Entity is Storage"));
				m_pLog->AddLine(strTmp);
			}

			nFpxDefault = Buf(m_nPos++);


			// BUG: #1112
			//streamStr = m_pWBuf->BufReadUniStr(m_nPos);
			streamStr = m_pWBuf->BufReadUniStr2(m_nPos,MAX_BUF_READ_STR);
			m_nPos += 2*((unsigned)_tcslen(streamStr)+1); // 2x because unicode

			strTmp.Format(_T("        Stream Name = [%s]"),(LPCTSTR)streamStr);
			m_pLog->AddLine(strTmp);


			// In the case of "storage", we decode the next 16 bytes as the class
			if (bFpxStorage) {

				// FIXME:
				// NOTE: Very strange reordering required here. Doesn't seem consistent
				//       This means that other fields are probably wrong as well (endian)
				strFpxStorageClsStr.Format(_T("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
					Buf(m_nPos+3),Buf(m_nPos+2),Buf(m_nPos+1),Buf(m_nPos+0),
					Buf(m_nPos+5),Buf(m_nPos+4),
					Buf(m_nPos+7),Buf(m_nPos+6),
					Buf(m_nPos+8),Buf(m_nPos+9),
					Buf(m_nPos+10),Buf(m_nPos+11),Buf(m_nPos+12),Buf(m_nPos+13),Buf(m_nPos+14),Buf(m_nPos+15) );
				m_nPos+= 16;
				strTmp.Format(_T("        Storage Class = [%s]"),(LPCTSTR)strFpxStorageClsStr);
				m_pLog->AddLine(strTmp);


			}

		}

		return 0;

	} else if (nFpxSegType == 2) {
		// Stream Data
		strTmp.Format(_T("    Segment: STREAM DATA"));
		m_pLog->AddLine(strTmp);

		nFpxStIndexCont = (Buf(m_nPos++)<<8) + Buf(m_nPos++);
		strTmp.Format(_T("      Index in Contents List = %u"),nFpxStIndexCont);
		m_pLog->AddLine(strTmp);

		nFpxStOffset = (Buf(m_nPos++)<<24) + (Buf(m_nPos++)<<16) + (Buf(m_nPos++)<<8) + Buf(m_nPos++);
		strTmp.Format(_T("      Offset in stream = %u (0x%08X)"),nFpxStOffset,nFpxStOffset);
		m_pLog->AddLine(strTmp);

		// Now decode the Property Set Header

		// NOTE: Should only decode this if we are doing first part of stream
		// TODO: How do we know this? First reference to index #?

		nFpxStWByteOrder = (Buf(m_nPos++)<<8) + Buf(m_nPos++);
		nFpxStWFormat = (Buf(m_nPos++)<<8) + Buf(m_nPos++);
		nFpxStDwOsVer = (Buf(m_nPos++)<<24) + (Buf(m_nPos++)<<16) + (Buf(m_nPos++)<<8) + Buf(m_nPos++);

		// FIXME:
		// NOTE: Very strange reordering required here. Doesn't seem consistent!
		//       This means that other fields are probably wrong as well (endian)
		strFpxStClsidStr.Format(_T("%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X"),
			Buf(m_nPos+3),Buf(m_nPos+2),Buf(m_nPos+1),Buf(m_nPos+0),
			Buf(m_nPos+5),Buf(m_nPos+4),
			Buf(m_nPos+7),Buf(m_nPos+6),
			Buf(m_nPos+8),Buf(m_nPos+9),
			Buf(m_nPos+10),Buf(m_nPos+11),Buf(m_nPos+12),Buf(m_nPos+13),Buf(m_nPos+14),Buf(m_nPos+15) );
		m_nPos+= 16;
		nFpxStRsvd = (Buf(m_nPos++)<<8) + Buf(m_nPos++);

		strTmp.Format(_T("      ByteOrder = 0x%04X"),nFpxStWByteOrder);
		m_pLog->AddLine(strTmp);

		strTmp.Format(_T("      Format = 0x%04X"),nFpxStWFormat);
		m_pLog->AddLine(strTmp);

		strTmp.Format(_T("      OSVer = 0x%08X"),nFpxStDwOsVer);
		m_pLog->AddLine(strTmp);

		strTmp.Format(_T("      clsid = %s"),(LPCTSTR)strFpxStClsidStr);
		m_pLog->AddLine(strTmp);

		strTmp.Format(_T("      reserved = 0x%08X"),nFpxStRsvd);
		m_pLog->AddLine(strTmp);

		// ....

		return 2;

	} else {
		strTmp.Format(_T("      Reserved Segment. Stopping."));
		m_pLog->AddLineErr(strTmp);
		return 1;
	}

}