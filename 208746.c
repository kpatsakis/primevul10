bool CjfifDecode::CompareSignature(bool bQuiet=false)
{
	CString strTmp;
	CString strHashOut;
	CString locationStr;

	unsigned ind;

	bool	bCurXsw = false;
	bool	bCurXmm = false;
	bool	bCurXmkr = false;
	bool	bCurXextrasw = false;	// EXIF Extra fields match software indicator
	bool	bCurXcomsw = false;		// EXIF COM field match software indicator
	bool	bCurXps = false;		// EXIF photoshop IRB present
	bool	bSrchXsw = false;
	bool	bSrchXswUsig = false;
	bool	bSrchXmmUsig = false;
	bool	bSrchUsig = false;
	bool	bMatchIjg = false;
	CString	sMatchIjgQual = _T("");

	ASSERT(m_strHash != _T("NONE"));
	ASSERT(m_strHashRot != _T("NONE"));

	if (bQuiet) { m_pLog->Disable(); }


	m_pLog->AddLine(_T(""));
	m_pLog->AddLineHdr(_T("*** Searching Compression Signatures ***"));
	m_pLog->AddLine(_T(""));

	// Output the hash
	strHashOut = _T("  Signature:           ");
	strHashOut += m_strHash;
	m_pLog->AddLine(strHashOut);
	strHashOut = _T("  Signature (Rotated): ");
	strHashOut += m_strHashRot;
	m_pLog->AddLine(strHashOut);

	strTmp.Format(_T("  File Offset:         %lu bytes"),m_pAppConfig->nPosStart);
	m_pLog->AddLine(strTmp);

	// Output the CSS
	strTmp.Format(_T("  Chroma subsampling:  %s"),(LPCTSTR)m_strImgQuantCss);
	m_pLog->AddLine(strTmp);


	// Calculate the final fields
	// Add Photoshop IRB entries
	// Note that we always add an entry to the m_strImgExtras even if
	// there are no photoshop tags detected. It will appear as "[PS]:[0/0]"
	strTmp = _T("");
	strTmp.Format(_T("[PS]:[%u/%u],"),m_nImgQualPhotoshopSa,m_nImgQualPhotoshopSfw);
	m_strImgExtras += strTmp;


	// --------------------------------------
	// Determine current entry fields

	// Note that some cameras/phones have an empty Make, but use the Model! (eg. Palm Treo)
	if ((m_strImgExifMake == _T("???")) && (m_strImgExifModel == _T("???"))) {
		m_pLog->AddLine(_T("  EXIF Make/Model:     NONE"));
		bCurXmm = false;
	} else {
		strTmp.Format(_T("  EXIF Make/Model:     OK   [%s] [%s]"),(LPCTSTR)m_strImgExifMake,(LPCTSTR)m_strImgExifModel);
		m_pLog->AddLine(strTmp);
		bCurXmm = true;
	}

	if (m_bImgExifMakernotes) {
		m_pLog->AddLine(_T("  EXIF Makernotes:     OK  "));
		bCurXmkr = true;
	} else {
		m_pLog->AddLine(_T("  EXIF Makernotes:     NONE"));
		bCurXmkr = false;
	}

	if (_tcslen(m_strSoftware) == 0) {
		m_pLog->AddLine(_T("  EXIF Software:       NONE"));
		bCurXsw = false;
	} else {
		strTmp.Format(_T("  EXIF Software:       OK   [%s]"),(LPCTSTR)m_strSoftware);
		m_pLog->AddLine(strTmp);

		// EXIF software field is non-empty
		bCurXsw = true;
	}

	m_pLog->AddLine(_T(""));

	// --------------------------------------
	// Determine search results


	// All of the rest of the search results require searching
	// through the database entries

	bSrchXswUsig = false;
	bSrchXmmUsig = false;
	bSrchUsig = false;
	bMatchIjg = false;
	sMatchIjgQual = _T("");

	unsigned nSigsInternal = theApp.m_pDbSigs->GetNumSigsInternal();
	unsigned nSigsExtra = theApp.m_pDbSigs->GetNumSigsExtra();

	strTmp.Format(_T("  Searching Compression Signatures: (%u built-in, %u user(*) )"),nSigsInternal,nSigsExtra);
	m_pLog->AddLine(strTmp);


	// Now in SIG version 0x01 and later, we are not including
	// the CSS in the signature. Therefore, we need to compare it
	// manually.

	bool curMatchMm;
	bool curMatchSw;
	bool curMatchSig;
	bool curMatchSigCss;


	// Check on Extras field
	// Noted that Canon EOS Viewer Utility (EVU) seems to convert RAWs with
	// the only identifying information being this:
	//  e.g. "[Canon.ImageType]:[CRW:EOS 300D DIGITAL CMOS RAW],_T("
	if (m_strImgExtras.Find(_T(")[Canon.ImageType]:[CRW:")) != -1) {
		bCurXextrasw = true;
	}
	if (m_strImgExtras.Find(_T("[Nikon1.Quality]:[RAW")) != -1) {
		bCurXextrasw = true;
	}
	if (m_strImgExtras.Find(_T("[Nikon2.Quality]:[RAW")) != -1) {
		bCurXextrasw = true;
	}
	if (m_strImgExtras.Find(_T("[Nikon3.Quality]:[RAW")) != -1) {
		bCurXextrasw = true;
	}
	if ((m_nImgQualPhotoshopSa != 0) || (m_nImgQualPhotoshopSfw != 0)) {
		bCurXps = true;
	}

	// Search for known COMment field indicators
	if (theApp.m_pDbSigs->SearchCom(m_strComment)) {
		bCurXcomsw = true;
	}

	m_pLog->AddLine(_T(""));
	m_pLog->AddLine(_T("          EXIF.Make / Software        EXIF.Model                            Quality           Subsamp Match?"));
	m_pLog->AddLine(_T("          -------------------------   -----------------------------------   ----------------  --------------"));

	CompSig pEntry;
	unsigned ind_max = theApp.m_pDbSigs->GetDBNumEntries();
	for (ind=0;ind<ind_max;ind++) {
		theApp.m_pDbSigs->GetDBEntry(ind,&pEntry);

		// Reset current entry state
		curMatchMm = false;
		curMatchSw = false;
		curMatchSig = false;
		curMatchSigCss = false;

		// Compare make/model (only for digicams)
		if ((pEntry.eEditor == ENUM_EDITOR_CAM) &&
			(bCurXmm == true) &&
			(pEntry.strXMake  == m_strImgExifMake) &&
			(pEntry.strXModel == m_strImgExifModel) )
		{
			curMatchMm = true;
		}

		// For software entries, do a loose search
		if ((pEntry.eEditor == ENUM_EDITOR_SW) &&
			(bCurXsw == true) &&
			(pEntry.strMSwTrim != _T("")) &&
			(m_strSoftware.Find(pEntry.strMSwTrim) != -1) )
		{
			// Software field matches known software string
			bSrchXsw = true;
			curMatchSw = true;
		}


		// Compare signature (and CSS for digicams)
		if ( (pEntry.strCSig == m_strHash) || (pEntry.strCSigRot == m_strHash) ||
			(pEntry.strCSig == m_strHashRot) || (pEntry.strCSigRot == m_strHashRot) )
		{
			curMatchSig = true;

			// If Database entry is for an editor, sig matches irrespective of CSS
			if (pEntry.eEditor == ENUM_EDITOR_SW) {
				bSrchUsig = true;
				curMatchSigCss = true; // FIXME: do I need this?

				// For special case of IJG
				if (pEntry.strMSwDisp == _T("IJG Library")) {
					bMatchIjg = true;
					sMatchIjgQual = pEntry.strUmQual;
				}

			} else {
				// Database entry is for a digicam, sig match only if CSS matches too
				if (pEntry.strXSubsamp == m_strImgQuantCss) {
					bSrchUsig = true;
					curMatchSigCss = true;
				} else {
					curMatchSigCss = false;
				}
				
			} // editor

		} else {
			// sig doesn't match
			curMatchSig = false;
			curMatchSigCss = false;
		}


		// For digicams:
		if (curMatchMm && curMatchSigCss) {
			bSrchXmmUsig = true;
		}
		// For software:
		if (curMatchSw && curMatchSig) {
			bSrchXswUsig = true;
		}

		if (theApp.m_pDbSigs->IsDBEntryUser(ind)) {
			locationStr = _T("*");
		} else {
			locationStr = _T(" ");
		}

		// Display entry if it is a good match
		if (curMatchSig) {
			if (pEntry.eEditor==ENUM_EDITOR_CAM) {
				strTmp.Format(_T("    %s%4s[%-25s] [%-35s] [%-16s] %-5s %-5s %-5s"),(LPCTSTR)locationStr,_T("CAM:"),
					(LPCTSTR)pEntry.strXMake.Left(25),(LPCTSTR)pEntry.strXModel.Left(35),(LPCTSTR)pEntry.strUmQual.Left(16),
					(curMatchSigCss?_T("Yes"):_T("No")),_T(""),_T(""));
			} else if (pEntry.eEditor==ENUM_EDITOR_SW) {
				strTmp.Format(_T("    %s%4s[%-25s]  %-35s  [%-16s] %-5s %-5s %-5s"),(LPCTSTR)locationStr,_T("SW :"),
					(LPCTSTR)pEntry.strMSwDisp.Left(25),_T(""),(LPCTSTR)pEntry.strUmQual.Left(16),
					_T(""),_T(""),_T(""));
			} else {
				strTmp.Format(_T("    %s%4s[%-25s] [%-35s] [%-16s] %-5s %-5s %-5s"),(LPCTSTR)locationStr,_T("?? :"),
					(LPCTSTR)pEntry.strXMake.Left(25),(LPCTSTR)pEntry.strXModel.Left(35),(LPCTSTR)pEntry.strUmQual.Left(16),
					_T(""),_T(""),_T(""));
			}
			if (curMatchMm || curMatchSw) {
				m_pLog->AddLineGood(strTmp);
			} else {
				m_pLog->AddLine(strTmp);
			}

		}


	} // loop through DB

	CString strSw;
	// If it matches an IJG signature, report other possible sources:
	if (bMatchIjg) {
		m_pLog->AddLine(_T(""));
		m_pLog->AddLine(_T("    The following IJG-based editors also match this signature:"));
		unsigned nIjgNum;
		CString strIjgSw;
		nIjgNum = theApp.m_pDbSigs->GetIjgNum();
		for (ind=0;ind<nIjgNum;ind++)
		{
			strIjgSw = theApp.m_pDbSigs->GetIjgEntry(ind);
			strTmp.Format(_T("     %4s[%-25s]  %-35s  [%-16s] %-5s %-5s %-5s"),_T("SW :"),
				(LPCTSTR)strIjgSw.Left(25),_T(""),(LPCTSTR)sMatchIjgQual.Left(16),
				_T(""),_T(""),_T(""));
			m_pLog->AddLine(strTmp);
		}
	}

	//m_pLog->AddLine(_T("          --------------------   -----------------------------------   ----------------  --------------"));
	m_pLog->AddLine(_T(""));

	if (bCurXps) {
		m_pLog->AddLine(_T("  NOTE: Photoshop IRB detected"));
	}
	if (bCurXextrasw) {
		m_pLog->AddLine(_T("  NOTE: Additional EXIF fields indicate software processing"));
	}
	if (bSrchXsw) {
		m_pLog->AddLine(_T("  NOTE: EXIF Software field recognized as from editor"));
	}
	if (bCurXcomsw) {
		m_pLog->AddLine(_T("  NOTE: JFIF COMMENT field is known software"));
	}



	// ============================================
	// Image Assessment Algorithm
	// ============================================

	bool bEditDefinite = false;
	bool bEditLikely = false;
	bool bEditNot = false;
	bool bEditNotUnknownSw = false;

	if (bCurXps) {
		bEditDefinite = true;
	}
	if (!bCurXmm) {
		bEditDefinite = true;
	}
	if (bCurXextrasw) {
		bEditDefinite = true;
	}
	if (bCurXcomsw) {
		bEditDefinite = true;
	}
	if (bSrchXsw) {
		// Software field matches known software string
		bEditDefinite = true;
	}
	if (theApp.m_pDbSigs->LookupExcMmIsEdit(m_strImgExifMake,m_strImgExifModel)) {
		// Make/Model is in exception list of ones that mark known software
		bEditDefinite = true;
	}

	if (!bCurXmkr) {
		// If we are missing maker notes, we are almost always dealing with
		// edited images. There are some known exceptions, so far:
		//  - Very old digicams
		//  - Certain camera phones
		//  - Blackberry 
		// Perhaps we can make an exception for particular digicams (based on
		// make/model) that this determination will not apply. This means that
		// we open up the doors for these files being edited and not caught.

		if (theApp.m_pDbSigs->LookupExcMmNoMkr(m_strImgExifMake,m_strImgExifModel)) {
			// This is a known exception!
		} else {
			bEditLikely = true;
		}
	}


	// Filter down remaining scenarios
	if (!bEditDefinite && !bEditLikely) {

		if (bSrchXmmUsig) {
			// DB cam signature matches DQT & make/model
			if (!bCurXsw) {
				// EXIF software field is empty
				//
				// We can now be pretty confident that this file has not
				// been edited by all the means that we are checking
				bEditNot = true;
			} else {
				// EXIF software field is set
				//
				// This field is often used by:
				//  - Software editors (edited)
				//  - RAW converter software (edited)
				//  - Digicams to indicate firmware (original)
				//  - Phones to indicate firmware (original)
				//
				// However, in generating bEditDefinite, we have already
				// checked for bSrchXsw which looked for known software
				// strings. Therefore, we will primarily be left with
				// firmware strings, etc.
				//
				// We will mark this as NOT EDITED but with caution of unknown SW field
				bEditNot = true;
				bEditNotUnknownSw = true;
			}
		} else {
			// No DB cam signature matches DQT & make/model
			// According to EXIF data, this file does not appear to be edited,
			// but no compression signatures in the database match this
			// particular make/model. Therefore, result is UNSURE.
		}

	}


	// Now make final assessment call



	// Determine if image has been processed/edited
	m_eImgEdited = EDITED_UNSET;
	if (bEditDefinite) {
		m_eImgEdited = EDITED_YES;
	} else if (bEditLikely) {
		m_eImgEdited = EDITED_YESPROB;
	} else if (bEditNot) {
		// Images that fall into this category will have:
		//  - No Photoshop tags
		//  - Make/Model is present
		//  - Makernotes present
		//  - No extra software tags (eg. IFD)
		//  - No comment field with known software
		//  - No software field or it does not match known software
		//  - Signature matches DB for this make/model
		m_eImgEdited = EDITED_NO;
	} else {
		// Images that fall into this category will have:
		//  - Same as EDITED_NO but:
		//  - Signature does not match DB for this make/model
		// In all likelihood, this image will in fact be original
		m_eImgEdited = EDITED_UNSURE;
	}




	// If the file offset is non-zero, then don't ask for submit or show assessment
	if (m_pAppConfig->nPosStart != 0) {
		m_pLog->AddLine(_T("  ASSESSMENT not done as file offset non-zero"));
		if (bQuiet) { m_pLog->Enable(); }
		return false;
	}

	// ============================================
	// Display final assessment
	// ============================================

	m_pLog->AddLine(_T("  Based on the analysis of compression characteristics and EXIF metadata:"));
	m_pLog->AddLine(_T(""));
	if (m_eImgEdited == EDITED_YES) {
		m_pLog->AddLine(_T("  ASSESSMENT: Class 1 - Image is processed/edited"));
	} else if (m_eImgEdited == EDITED_YESPROB) {
		m_pLog->AddLine(_T("  ASSESSMENT: Class 2 - Image has high probability of being processed/edited"));
	} else if (m_eImgEdited == EDITED_NO) {
		m_pLog->AddLine(_T("  ASSESSMENT: Class 3 - Image has high probability of being original"));
		// In case the EXIF Software field was detected, 
		if (bEditNotUnknownSw) {
			m_pLog->AddLine(_T("              Note that EXIF Software field is set (typically contains Firmware version)"));
		}
	} else if (m_eImgEdited == EDITED_UNSURE) {
		m_pLog->AddLine(_T("  ASSESSMENT: Class 4 - Uncertain if processed or original"));
		m_pLog->AddLine(_T("              While the EXIF fields indicate original, no compression signatures "));
		m_pLog->AddLine(_T("              in the current database were found matching this make/model"));
	} else {
		m_pLog->AddLineErr(_T("  ASSESSMENT: *** Failed to complete ***"));
	}
	m_pLog->AddLine(_T(""));



	// Determine if user should add entry to DB
	bool bDbReqAdd = false;		// Ask user to add
	bool bDbReqAddAuto = false;	// Automatically add (in batch operation)


	// TODO: This section should be rewritten to reduce complexity

	m_eDbReqSuggest = DB_ADD_SUGGEST_UNSET;
	if (m_eImgEdited == EDITED_NO) {
		bDbReqAdd = false;
		m_eDbReqSuggest = DB_ADD_SUGGEST_CAM;
	} else if (m_eImgEdited == EDITED_UNSURE) {
		bDbReqAdd = true;
		bDbReqAddAuto = true;
		m_eDbReqSuggest = DB_ADD_SUGGEST_CAM;
		m_pLog->AddLine(_T("  Appears to be new signature for known camera."));
		m_pLog->AddLine(_T("  If the camera/software doesn't appear in list above,"));
		m_pLog->AddLineWarn(_T("  PLEASE ADD TO DATABASE with [Tools->Add Camera to DB]"));
	} else if (bCurXps && bSrchUsig) {
		// Photoshop and we already have sig
		bDbReqAdd = false;
		m_eDbReqSuggest = DB_ADD_SUGGEST_SW;
	} else if (bCurXps && !bSrchUsig) {
		// Photoshop and we don't already have sig
		bDbReqAdd = true;
		bDbReqAddAuto = true;
		m_eDbReqSuggest = DB_ADD_SUGGEST_SW;
		m_pLog->AddLine(_T("  Appears to be new signature for Photoshop."));
		m_pLog->AddLine(_T("  If it doesn't appear in list above,"));
		m_pLog->AddLineWarn(_T("  PLEASE ADD TO DATABASE with [Tools->Add Camera to DB]"));
	} else if (bCurXsw && bSrchXsw && bSrchXswUsig) {
		bDbReqAdd = false;
		m_eDbReqSuggest = DB_ADD_SUGGEST_SW;
	} else if (bCurXextrasw) {
		bDbReqAdd = false;
		m_eDbReqSuggest = DB_ADD_SUGGEST_SW;
	} else if (bCurXsw && bSrchXsw && !bSrchXswUsig) {
		bDbReqAdd = true;
		//bDbReqAddAuto = true;
		m_eDbReqSuggest = DB_ADD_SUGGEST_SW;
		m_pLog->AddLine(_T("  Appears to be new signature for known software."));
		m_pLog->AddLine(_T("  If the camera/software doesn't appear in list above,"));
		m_pLog->AddLineWarn(_T("  PLEASE ADD TO DATABASE with [Tools->Add Camera to DB]"));
	} else if (bCurXmm && bCurXmkr && !bSrchXsw && !bSrchXmmUsig) {
		// unsure if cam, so ask user
		bDbReqAdd = true;
		bDbReqAddAuto = true; 
		m_eDbReqSuggest = DB_ADD_SUGGEST_CAM;
		m_pLog->AddLine(_T("  This may be a new camera for the database."));
		m_pLog->AddLine(_T("  If this file is original, and camera doesn't appear in list above,"));
		m_pLog->AddLineWarn(_T("  PLEASE ADD TO DATABASE with [Tools->Add Camera to DB]"));
	} else if (!bCurXmm && !bCurXmkr && !bSrchXsw) {
		// unsure if SW, so ask user
		bDbReqAdd = true;
		m_eDbReqSuggest = DB_ADD_SUGGEST_SW;
		m_pLog->AddLine(_T("  This may be a new software editor for the database."));
		m_pLog->AddLine(_T("  If this file is processed, and editor doesn't appear in list above,"));
		m_pLog->AddLineWarn(_T("  PLEASE ADD TO DATABASE with [Tools->Add Camera to DB]"));
	}


	m_pLog->AddLine(_T(""));

	// -----------------------------------------------------------

	if (bQuiet) { m_pLog->Enable(); }

#ifdef BATCH_DO_DBSUBMIT_ALL
	bDbReqAddAuto = true;
#endif

	// Return a value that indicates whether or not we should add this
	// entry to the database
	return bDbReqAddAuto;

}