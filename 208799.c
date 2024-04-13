unsigned CjfifDecode::DecodeExifIfd(CString strIfd,unsigned nPosExifStart,unsigned nStartIfdPtr)
{
	// Temp variables
	bool			bRet;
	CString			strTmp;
	CStr2			strRetVal;
	CString			strValTmp;
	float			fValReal;
	CString			strMaker;

	// Display output variables
	CString			strFull;
	CString			strValOut;
	BOOL			bExtraDecode;

	// Primary IFD variables
	char		acIfdValOffsetStr[5];
	unsigned	nIfdDirLen;
	unsigned	nIfdTagVal;
	unsigned	nIfdFormat;
	unsigned	nIfdNumComps;
	bool		nIfdTagUnknown;


	unsigned	nCompsToDisplay;			// Maximum number of values to capture for display
	unsigned	anValues[MAX_anValues];		// Array of decoded values (Uint32)
	signed		anValuesS[MAX_anValues];	// Array of decoded values (Int32)
	float		afValues[MAX_anValues];		// Array of decoded values (float)
	unsigned	nIfdOffset;					// First DWORD decode, usually offset


	// Clear values array
	for (unsigned ind=0;ind<MAX_anValues;ind++) {
		anValues[ind] = 0;
		anValuesS[ind] = 0;
		afValues[ind] = 0;
	}

	// ==========================================================================
	// Process IFD directory header
	// ==========================================================================

	// Move the file pointer to the start of the IFD
	m_nPos = nPosExifStart+nStartIfdPtr;

	strTmp.Format(_T("  EXIF %s @ Absolute 0x%08X"),(LPCTSTR)strIfd,m_nPos);
	m_pLog->AddLine(strTmp);

	////////////

	// NOTE: Nikon type 3 starts out with the ASCII string "Nikon\0"
	//       before the rest of the items.
	// TODO: need to process type1,type2,type3
	// see: http://www.gvsoft.homedns.org/exif/makernote-nikon.html

	strTmp.Format(_T("strIfd=[%s] m_strImgExifMake=[%s]"),(LPCTSTR)strIfd,(LPCTSTR)m_strImgExifMake);
	DbgAddLine(strTmp);


	// If this is the MakerNotes section, then we may want to skip
	// altogether. Check to see if we are configured to process this
	// section or if it is a supported manufacturer.

	if (strIfd == _T("MakerIFD"))
	{
		// Mark the image as containing Makernotes
		m_bImgExifMakernotes = true;

		if (!m_pAppConfig->bDecodeMaker) {
			strTmp.Format(_T("    Makernote decode option not enabled."));
			m_pLog->AddLine(strTmp);

			// If user didn't enable makernote decode, don't exit, just
			// hide output. We still want to get at some info (such as Quality setting).
			// At end, we'll need to re-enable it again.
			m_pLog->Disable();
		}

		// If this Make is not supported, we'll need to exit
		if (!m_bImgExifMakeSupported) {
			strTmp.Format(_T("    Makernotes not yet supported for [%s]"),(LPCTSTR)m_strImgExifMake);
			m_pLog->AddLine(strTmp);

			m_pLog->Enable();
			return 2;
		}

		// Determine the sub-type of the Maker field (if applicable)
		// and advance the m_nPos pointer past the custom header.
		// This call uses the class members: Buf(),m_nPos
		if (!DecodeMakerSubType())
		{
			// If the subtype decode failed, skip the processing
			m_pLog->Enable();
			return 2;
		}

	}

	// ==========================================================================
	// Process IFD directory entries
	// ==========================================================================

	CString strIfdTag;

	// =========== EXIF IFD Header (Start) ===========
	// - Defined in Exif 2.2 Standard (JEITA CP-3451) section 4.6.2 
	// - Contents (2 bytes total)
	//   - Number of fields (2 bytes)

	nIfdDirLen = ReadSwap2(m_nPos);
	m_nPos+=2;
	strTmp.Format(_T("    Dir Length = 0x%04X"),nIfdDirLen);
	m_pLog->AddLine(strTmp);

	// =========== EXIF IFD Header (End) ===========


	// Start of IFD processing
	// Step through each IFD entry and determine the type and
	// decode accordingly.
	for (unsigned nIfdEntryInd=0;nIfdEntryInd<nIfdDirLen;nIfdEntryInd++)
	{

		// By default, show single-line value summary
		// bExtraDecode is used to indicate that additional special
		// parsing output is available for this entry
		bExtraDecode = FALSE;

		strTmp.Format(_T("    Entry #%02u:"),nIfdEntryInd);
		DbgAddLine(strTmp);

		// =========== EXIF IFD Interoperability entry (Start) ===========
		// - Defined in Exif 2.2 Standard (JEITA CP-3451) section 4.6.2 
		// - Contents (12 bytes total)
		//   - Tag (2 bytes)
		//   - Type (2 bytes)
		//   - Count (4 bytes)
		//   - Value Offset (4 bytes)

		// Read Tag #
		nIfdTagVal = ReadSwap2(m_nPos);
		m_nPos+=2;
		nIfdTagUnknown = false;
		strIfdTag = LookupExifTag(strIfd,nIfdTagVal,nIfdTagUnknown);
		strTmp.Format(_T("      Tag # = 0x%04X = [%s]"),nIfdTagVal,(LPCTSTR)strIfdTag);
		DbgAddLine(strTmp);

		// Read Format (or Type)
		nIfdFormat = ReadSwap2(m_nPos);
		m_nPos+=2;
		strTmp.Format(_T("      Format # = 0x%04X"),nIfdFormat);
		DbgAddLine(strTmp);

		// Read number of Components
		nIfdNumComps = ReadSwap4(m_nPos);
		m_nPos+=4;
		strTmp.Format(_T("      # Comps = 0x%08X"),nIfdNumComps);
		DbgAddLine(strTmp);

		// Check to see how many components have been listed.
		// This helps trap errors in corrupted IFD segments, otherwise
		// we will hang trying to decode millions of entries!
		// See issue & testcase #1148
		if (nIfdNumComps > 4000) {
			// Warn user that we have clippped the component list.
			// Note that this condition is only relevant when we are
			// processing the general array fields. Fields such as MakerNote
			// will also enter this condition so we shouldn't warn in those cases.
			//
			// TODO: Defer this warning message until after we are sure that we
			// didn't handle the large dataset elsewhere.
			// For now, only report this warning if we are not processing MakerNote
			if (strIfdTag.Compare(_T("MakerNote"))!=0) {
				strTmp.Format(_T("      Excessive # components (%u). Limiting to first 4000."),nIfdNumComps);
				m_pLog->AddLineWarn(strTmp);
			}
			nIfdNumComps = 4000;
		}

		// Read Component Value / Offset
		// We first treat it as a string and then re-interpret it as an integer

		// ... first as a string (just in case length <=4)
		for (unsigned i=0;i<4;i++) {
			acIfdValOffsetStr[i] = (char)Buf(m_nPos+i);
		}
		acIfdValOffsetStr[4] = '\0';

		// ... now as an unsigned value
		// This assignment is general-purpose, typically used when
		// we know that the IFD Value/Offset is just an offset
		nIfdOffset = ReadSwap4(m_nPos);
		strTmp.Format(_T("      # Val/Offset = 0x%08X"),nIfdOffset);
		DbgAddLine(strTmp);

		// =========== EXIF IFD Interoperability entry (End) ===========


		// ==========================================================================
		// Extract the IFD component entries
		// ==========================================================================

		// The EXIF IFD entries can appear in a wide range of
		// formats / data types. The formats that have been
		// publicly documented include:
		//   EXIF_FORMAT_BYTE       =  1,
		//   EXIF_FORMAT_ASCII      =  2,
		//   EXIF_FORMAT_SHORT      =  3,
		//   EXIF_FORMAT_LONG       =  4,
		//   EXIF_FORMAT_RATIONAL   =  5,
		//   EXIF_FORMAT_SBYTE      =  6,
		//   EXIF_FORMAT_UNDEFINED  =  7,
		//   EXIF_FORMAT_SSHORT     =  8,
		//   EXIF_FORMAT_SLONG      =  9,
		//   EXIF_FORMAT_SRATIONAL  = 10,
		//   EXIF_FORMAT_FLOAT      = 11,
		//   EXIF_FORMAT_DOUBLE     = 12

		// The IFD variable formatter logic operates in two stages:
		// In the first stage, the format type is decoded, which results
		// in a generic decode for the IFD entry. Then, we start a second
		// stage which re-interprets the values for a number of known
		// special types.

		switch(nIfdFormat)
		{

		// ----------------------------------------
		// --- IFD Entry Type: Unsigned Byte
		// ----------------------------------------
		case 1:
			strFull = _T("        Unsigned Byte=[");
			strValOut = _T("");

			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);

			// If only a single value, use decimal, else use hex
			if (nIfdNumComps == 1) {
				anValues[0] = Buf(m_nPos+0);
				strTmp.Format(_T("%u"),anValues[0]);
				strValOut += strTmp;
			} else {
				for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++)
				{
					if (nIfdNumComps <= 4) {
						// Components fit inside 4B inline region
						anValues[nInd] = Buf(m_nPos+nInd);
					} else {
						// Since the components don't fit inside 4B inline region
						// we need to dereference
						anValues[nInd] = Buf(nPosExifStart+nIfdOffset+nInd);
					}
				}
				strValOut = PrintAsHex8(anValues,nIfdNumComps);
			}
			strFull += strValOut;
			strFull += _T("]");
			DbgAddLine(strFull);
			break;


		// ----------------------------------------
		// --- IFD Entry Type: ASCII string
		// ----------------------------------------
		case 2:
			strFull = _T("        String=");
			strValOut = _T("");
			char cVal;
			BYTE nVal;

			// Limit display output
			// TODO: Decide what an appropriate string limit would be
			nCompsToDisplay = min(250,nIfdNumComps);

			for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++)
			{
				if (nIfdNumComps<=4) {
					nVal = acIfdValOffsetStr[nInd];
				}
				else
				{
					// TODO: See if this can be migrated to the "custom decode"
					// section later in the code. Decoding makernotes here is
					// less desirable but unfortunately some Nikon makernotes use
					// a non-standard offset value.
					if ( (strIfd == _T("MakerIFD")) && (m_strImgExifMake == _T("NIKON")) && (m_nImgExifMakeSubtype == 3) )
					{
						// It seems that pointers in the Nikon Makernotes are
						// done relative to the start of Maker IFD
						// But why 10? Is this 10 = 18-8?
						nVal = Buf(nPosExifStart+m_nImgExifMakerPtr+nIfdOffset+10+nInd);
					} else if ( (strIfd == _T("MakerIFD")) && (m_strImgExifMake == _T("NIKON")) )
					{
						// It seems that pointers in the Nikon Makernotes are
						// done relative to the start of Maker IFD
						nVal = Buf(nPosExifStart+nIfdOffset+0+nInd);
					} else {
						// Canon Makernotes seem to be relative to the start
						// of the EXIF IFD
						nVal = Buf(nPosExifStart+nIfdOffset+nInd);
					}
				}

				// Just in case the string has been null-terminated early
				// or we have garbage, replace with '.'
				// TODO: Clean this up
				if (nVal != 0) {
					cVal = (char)nVal;
					if (!isprint(nVal)) {
						cVal = '.';
					}
					strValOut += cVal;
				}

			}
			strFull += strValOut;
			DbgAddLine(strFull);

			// TODO: Ideally, we would use a different string for display
			// purposes that is wrapped in quotes. Currently "strValOut" is used
			// in other sections of code (eg. in assignment to EXIF Make/Model/Software, etc.)
			// so we don't want to affect that.

			break;


		// ----------------------------------------
		// --- IFD Entry Type: Unsigned Short (2 bytes)
		// ----------------------------------------
		case 3:
			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);

			// Unsigned Short (2 bytes)
			if (nIfdNumComps == 1) {
				strFull = _T("        Unsigned Short=[");
				// TODO: Confirm endianness is correct here
				// Refer to Exif2-2 spec, page 14.
				// Currently selecting 2 byte conversion from [1:0] out of [3:0]
				anValues[0] = ReadSwap2(m_nPos);
				strValOut.Format(_T("%u"),anValues[0]);
				strFull += strValOut;
				strFull += _T("]");
				DbgAddLine(strFull);
			} else if (nIfdNumComps == 2) {
				strFull = _T("        Unsigned Short=[");
				// 2 unsigned shorts in 1 word
				anValues[0] = ReadSwap2(m_nPos+0);
				anValues[1] = ReadSwap2(m_nPos+2);
				strValOut.Format(_T("%u, %u"),anValues[0],anValues[1]);
				strFull += strValOut;
				strFull += _T("]");
				DbgAddLine(strFull);
			} else if (nIfdNumComps > MAX_IFD_COMPS) {
				strValTmp.Format(_T("    Unsigned Short=[Too many entries (%u) to display]"),nIfdNumComps);
				DbgAddLine(strValTmp);
				strValOut.Format(_T("[Too many entries (%u) to display]"),nIfdNumComps);
			} else {
				// Try to handle multiple entries... note that this
				// is used by the Maker notes IFD decode

				strValOut = _T("");
				strFull = _T("        Unsigned Short=[");
				for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++) {
					if (nInd!=0)	{ strValOut += _T(", "); }
					anValues[nInd] = ReadSwap2(nPosExifStart+nIfdOffset+(2*nInd));
					strValTmp.Format(_T("%u"),anValues[nInd]);
					strValOut += strValTmp;
				}
				strFull += strValOut;
				strFull += _T("]");
				DbgAddLine(strFull);


			}
			break;


		// ----------------------------------------
		// --- IFD Entry Type: Unsigned Long (4 bytes)
		// ----------------------------------------
		case 4:
			strFull = _T("        Unsigned Long=[");
			strValOut = _T("");

			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);

			for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++)
			{
				if (nIfdNumComps == 1) {
					// Components fit inside 4B inline region
					anValues[nInd] = ReadSwap4(m_nPos+(nInd*4));
				} else {
					// Since the components don't fit inside 4B inline region
					// we need to dereference
					anValues[nInd] = ReadSwap4(nPosExifStart+nIfdOffset+(nInd*4));
				}
			}
			strValOut = PrintAsHex32(anValues,nIfdNumComps);
			// If we only have a single component, then display both the hex and decimal
			if (nCompsToDisplay==1) {
				strTmp.Format(_T("%s / %u"),(LPCTSTR)strValOut,anValues[0]);
				strValOut = strTmp;
			}
			break;


		// ----------------------------------------
		// --- IFD Entry Type: Unsigned Rational (8 bytes)
		// ----------------------------------------
		case 5:
			// Unsigned Rational
			strFull = _T("        Unsigned Rational=[");
			strValOut = _T("");

			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);

			for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++)
			{
				if (nInd!=0)	{ strValOut += _T(", "); }
				strValTmp = DecodeValFraction(nPosExifStart+nIfdOffset+(nInd*8));
				bRet = DecodeValRational(nPosExifStart+nIfdOffset+(nInd*8),fValReal);
				afValues[nInd] = fValReal;
				strValOut += strValTmp;
			}
			strFull += strValOut;
			strFull += _T("]");
			DbgAddLine(strFull);
			break;

		// ----------------------------------------
		// --- IFD Entry Type: Undefined (?)
		// ----------------------------------------
		case 7:
			// Undefined -- assume 1 word long
			// This is supposed to be a series of 8-bit bytes
			// It is usually used for 32-bit pointers (in case of offsets), but could
			// also represent ExifVersion, etc.
			strFull = _T("        Undefined=[");
			strValOut = _T("");

			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);
			
			if (nIfdNumComps <= 4) {
				// This format is not defined, so output as hex for now
				for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++) {
					anValues[nInd] = Buf(m_nPos+nInd);
				}
				strValOut = PrintAsHex8(anValues,nIfdNumComps);
				strFull += strValOut;

			} else {

				// Dereference pointer
				for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++) {
					anValues[nInd] = Buf(nPosExifStart+nIfdOffset+nInd);
				}
				strValOut = PrintAsHex8(anValues,nIfdNumComps);
				strFull += strValOut;
			}
			strFull += _T("]");
			DbgAddLine(strFull);

			break;


		// ----------------------------------------
		// --- IFD Entry Type: Signed Short (2 bytes)
		// ----------------------------------------
		case 8:
			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);

			// Signed Short (2 bytes)
			if (nIfdNumComps == 1) {
				strFull = _T("        Signed Short=[");
				// TODO: Confirm endianness is correct here
				// Refer to Exif2-2 spec, page 14.
				// Currently selecting 2 byte conversion from [1:0] out of [3:0]

				// TODO: Ensure that ReadSwap2 handles signed notation properly
				anValuesS[0] = ReadSwap2(m_nPos);
				strValOut.Format(_T("%d"),anValuesS[0]);
				strFull += strValOut;
				strFull += _T("]");
				DbgAddLine(strFull);
			} else if (nIfdNumComps == 2) {
				strFull = _T("        Signed Short=[");
				// 2 signed shorts in 1 word

				// TODO: Ensure that ReadSwap2 handles signed notation properly
				anValuesS[0] = ReadSwap2(m_nPos+0);
				anValuesS[1] = ReadSwap2(m_nPos+2);
				strValOut.Format(_T("%d, %d"),anValuesS[0],anValuesS[0]);
				strFull += strValOut;
				strFull += _T("]");
				DbgAddLine(strFull);
			} else if (nIfdNumComps > MAX_IFD_COMPS) {
				// Only print it out if it has less than MAX_IFD_COMPS entries
				strValTmp.Format(_T("    Signed Short=[Too many entries (%u) to display]"),nIfdNumComps);
				DbgAddLine(strValTmp);
				strValOut.Format(_T("[Too many entries (%u) to display]"),nIfdNumComps);
			} else {
				// Try to handle multiple entries... note that this
				// is used by the Maker notes IFD decode

				// Note that we don't call LookupMakerCanonTag() here
				// as that is only needed for the "unsigned short", not
				// "signed short".
				strValOut = _T("");
				strFull = _T("        Signed Short=[");
				for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++) {
					if (nInd!=0)	{ strValOut += _T(", "); }
					anValuesS[nInd] = ReadSwap2(nPosExifStart+nIfdOffset+(2*nInd));
					strValTmp.Format(_T("%d"),anValuesS[nInd]);
					strValOut += strValTmp;
				}
				strFull += strValOut;
				strFull += _T("]");
				DbgAddLine(strFull);

			}
			break;



		// ----------------------------------------
		// --- IFD Entry Type: Signed Rational (8 bytes)
		// ----------------------------------------
		case 10:
			// Signed Rational
			strFull = _T("        Signed Rational=[");
			strValOut = _T("");

			// Limit display output
			nCompsToDisplay = min(MAX_anValues,nIfdNumComps);

			for (unsigned nInd=0;nInd<nCompsToDisplay;nInd++)
			{
				if (nInd!=0)	{ strValOut += _T(", "); }
				strValTmp = DecodeValFraction(nPosExifStart+nIfdOffset+(nInd*8));
				bRet = DecodeValRational(nPosExifStart+nIfdOffset+(nInd*8),fValReal);
				afValues[nInd] = fValReal;
				strValOut += strValTmp;
			}
			strFull += strValOut;
			strFull += _T("]");
			DbgAddLine(strFull);
			break;

		default:
			strTmp.Format(_T("ERROR: Unsupported format [%d]"),nIfdFormat);
			anValues[0] = ReadSwap4(m_nPos);
			strValOut.Format(_T("0x%08X???"),anValues[0]);
			m_pLog->Enable();
			return 2;
			break;
		} // switch nIfdTagVal



		// ==========================================================================
		// Custom Value String decodes
		// ==========================================================================

		// At this point we might re-format the values, thereby
		// overriding the default strValOut. We have access to the
		//   anValues[]  (array of unsigned int)
		//   anValuesS[] (array of signed int)
		//   afValues[]  (array of float)

		// Re-format special output items
		//   This will override "strValOut" that may have previously been defined

		if ((strIfdTag == _T("GPSLatitude")) ||
			(strIfdTag == _T("GPSLongitude"))) {
			bRet = PrintValGPS(nIfdNumComps,afValues[0],afValues[1],afValues[2],strValOut);
		} else if (strIfdTag == _T("GPSVersionID")) {
			strValOut.Format(_T("%u.%u.%u.%u"),anValues[0],anValues[1],anValues[2],anValues[3]);
		} else if (strIfdTag == _T("GPSAltitudeRef")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Above Sea Level"); break;
				case 1 : strValOut = _T("Below Sea Level"); break;
			}
		} else if (strIfdTag == _T("GPSStatus")) {
			switch (acIfdValOffsetStr[0]) {
				case 'A' : strValOut = _T("Measurement in progress"); break;
				case 'V' : strValOut = _T("Measurement Interoperability"); break;
			}
		} else if (strIfdTag == _T("GPSMeasureMode")) {
			switch (acIfdValOffsetStr[0]) {
				case '2' : strValOut = _T("2-dimensional"); break;
				case '3' : strValOut = _T("3-dimensional"); break;
			}
		} else if ((strIfdTag == _T("GPSSpeedRef")) ||
					(strIfdTag == _T("GPSDestDistanceRef"))) {
			switch (acIfdValOffsetStr[0]) {
				case 'K' : strValOut = _T("km/h"); break;
				case 'M' : strValOut = _T("mph"); break;
				case 'N' : strValOut = _T("knots"); break;
			}
		} else if ((strIfdTag == _T("GPSTrackRef")) ||
					(strIfdTag == _T("GPSImgDirectionRef")) ||
					(strIfdTag == _T("GPSDestBearingRef"))) {
			switch (acIfdValOffsetStr[0]) {
				case 'T' : strValOut = _T("True direction"); break;
				case 'M' : strValOut = _T("Magnetic direction"); break;
			}
		} else if (strIfdTag == _T("GPSDifferential")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Measurement without differential correction"); break;
				case 1 : strValOut = _T("Differential correction applied"); break;
			}
		} else if (strIfdTag == _T("GPSAltitude")) {
			strValOut.Format(_T("%.3f m"),afValues[0]);
		} else if (strIfdTag == _T("GPSSpeed")) {
			strValOut.Format(_T("%.3f"),afValues[0]);
		} else if (strIfdTag == _T("GPSTimeStamp")) {
			strValOut.Format(_T("%.0f:%.0f:%.2f"),afValues[0],afValues[1],afValues[2]);
		} else if (strIfdTag == _T("GPSTrack")) {
			strValOut.Format(_T("%.2f"),afValues[0]);
		} else if (strIfdTag == _T("GPSDOP")) {
			strValOut.Format(_T("%.4f"),afValues[0]);
		}


		if (strIfdTag == _T("Compression")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("None"); break;
				case 6 : strValOut = _T("JPEG"); break;
			}
		} else if (strIfdTag == _T("ExposureTime")) {
			// Assume only one
			strValTmp = strValOut;
			strValOut.Format(_T("%s s"),(LPCTSTR)strValTmp);
		} else if (strIfdTag == _T("FNumber")) {
			// Assume only one
			strValOut.Format(_T("F%.1f"),afValues[0]);
		} else if (strIfdTag == _T("FocalLength")) {
			// Assume only one
			strValOut.Format(_T("%.0f mm"),afValues[0]);
		} else if (strIfdTag == _T("ExposureBiasValue")) {
			// Assume only one
			// TODO: Need to test negative numbers
			strValOut.Format(_T("%0.2f eV"),afValues[0]);
		} else if (strIfdTag == _T("ExifVersion")) {
			// Assume only one
			strValOut.Format(_T("%c%c.%c%c"),anValues[0],anValues[1],anValues[2],anValues[3]);
		} else if (strIfdTag == _T("FlashPixVersion")) {
			// Assume only one
			strValOut.Format(_T("%c%c.%c%c"),anValues[0],anValues[1],anValues[2],anValues[3]);
		} else if (strIfdTag == _T("PhotometricInterpretation")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("Monochrome"); break;
				case 2 : strValOut = _T("RGB"); break;
				case 6 : strValOut = _T("YCbCr"); break;
			}
		} else if (strIfdTag == _T("Orientation")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("1 = Row 0: top, Col 0: left"); break;
				case 2 : strValOut = _T("2 = Row 0: top, Col 0: right"); break;
				case 3 : strValOut = _T("3 = Row 0: bottom, Col 0: right"); break;
				case 4 : strValOut = _T("4 = Row 0: bottom, Col 0: left"); break;
				case 5 : strValOut = _T("5 = Row 0: left, Col 0: top"); break;
				case 6 : strValOut = _T("6 = Row 0: right, Col 0: top"); break;
				case 7 : strValOut = _T("7 = Row 0: right, Col 0: bottom"); break;
				case 8 : strValOut = _T("8 = Row 0: left, Col 0: bottom"); break;
			}
		} else if (strIfdTag == _T("PlanarConfiguration")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("Chunky format"); break;
				case 2 : strValOut = _T("Planar format"); break;
			}
		} else if (strIfdTag == _T("YCbCrSubSampling")) {
			switch (anValues[0]*65536 + anValues[1]) {
				case 0x00020001 : strValOut = _T("4:2:2"); break;
				case 0x00020002 : strValOut = _T("4:2:0"); break;
			}
		} else if (strIfdTag == _T("YCbCrPositioning")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("Centered"); break;
				case 2 : strValOut = _T("Co-sited"); break;
			}
		} else if (strIfdTag == _T("ResolutionUnit")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("None"); break;
				case 2 : strValOut = _T("Inch"); break;
				case 3 : strValOut = _T("Centimeter"); break;
			}
		} else if (strIfdTag == _T("FocalPlaneResolutionUnit")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("None"); break;
				case 2 : strValOut = _T("Inch"); break;
				case 3 : strValOut = _T("Centimeter"); break;
			}
		} else if (strIfdTag == _T("ColorSpace")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("sRGB"); break;
				case 0xFFFF : strValOut = _T("Uncalibrated"); break;
			}
		} else if (strIfdTag == _T("ComponentsConfiguration")) {
			// Undefined type, assume 4 bytes
			strValOut = _T("[");
			for (unsigned vind=0;vind<4;vind++) {
				if (vind != 0) { strValOut += _T(" "); }
				switch (anValues[vind]) {
					case 0 : strValOut += _T("."); break;
					case 1 : strValOut += _T("Y"); break;
					case 2 : strValOut += _T("Cb"); break;
					case 3 : strValOut += _T("Cr"); break;
					case 4 : strValOut += _T("R"); break;
					case 5 : strValOut += _T("G"); break;
					case 6 : strValOut += _T("B"); break;
					default : strValOut += _T("?"); break;
				}
			}
			strValOut += _T("]");

		} else if ( (strIfdTag == _T("XPTitle")) ||
					(strIfdTag == _T("XPComment")) ||
					(strIfdTag == _T("XPAuthor")) ||
					(strIfdTag == _T("XPKeywords")) ||
					(strIfdTag == _T("XPSubject")) ) {
			strValOut = _T("\"");
			CString		strVal;
			strVal = m_pWBuf->BufReadUniStr2(nPosExifStart+nIfdOffset,nIfdNumComps);
			strValOut += strVal;
			strValOut += _T("\"");

		} else if (strIfdTag == _T("UserComment")) {
			// Character code
			unsigned anCharCode[8];
			for (unsigned vInd=0;vInd<8;vInd++) {
				anCharCode[vInd] = Buf(nPosExifStart+nIfdOffset+0+vInd);
			}
			// Actual string
			strValOut = _T("\"");
			bool bDone = false;
			char cTmp;

			for (unsigned vInd=0;(vInd<nIfdNumComps-8)&&(!bDone);vInd++) {
				cTmp = (char)Buf(nPosExifStart+nIfdOffset+8+vInd);
				if (cTmp == 0) { bDone = true; } else {	strValOut += cTmp;	}
			}
			strValOut += _T("\"");
		} else if (strIfdTag == _T("MeteringMode")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Unknown"); break;
				case 1 : strValOut = _T("Average"); break;
				case 2 : strValOut = _T("CenterWeightedAverage"); break;
				case 3 : strValOut = _T("Spot"); break;
				case 4 : strValOut = _T("MultiSpot"); break;
				case 5 : strValOut = _T("Pattern"); break;
				case 6 : strValOut = _T("Partial"); break;
				case 255 : strValOut = _T("Other"); break;
			}
		} else if (strIfdTag == _T("ExposureProgram")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Not defined"); break;
				case 1 : strValOut = _T("Manual"); break;
				case 2 : strValOut = _T("Normal program"); break;
				case 3 : strValOut = _T("Aperture priority"); break;
				case 4 : strValOut = _T("Shutter priority"); break;
				case 5 : strValOut = _T("Creative program (depth of field)"); break;
				case 6 : strValOut = _T("Action program (fast shutter speed)"); break;
				case 7 : strValOut = _T("Portrait mode"); break;
				case 8 : strValOut = _T("Landscape mode"); break;
			}
		} else if (strIfdTag == _T("Flash")) {
			switch (anValues[0] & 1) {
				case 0 : strValOut = _T("Flash did not fire"); break;
				case 1 : strValOut = _T("Flash fired"); break;
			}
			// TODO: Add other bitfields?
		} else if (strIfdTag == _T("SensingMethod")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("Not defined"); break;
				case 2 : strValOut = _T("One-chip color area sensor"); break;
				case 3 : strValOut = _T("Two-chip color area sensor"); break;
				case 4 : strValOut = _T("Three-chip color area sensor"); break;
				case 5 : strValOut = _T("Color sequential area sensor"); break;
				case 7 : strValOut = _T("Trilinear sensor"); break;
				case 8 : strValOut = _T("Color sequential linear sensor"); break;
			}
		} else if (strIfdTag == _T("FileSource")) {
			switch (anValues[0]) {
				case 3 : strValOut = _T("DSC"); break;
			}
		} else if (strIfdTag == _T("CustomRendered")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Normal process"); break;
				case 1 : strValOut = _T("Custom process"); break;
			}
		} else if (strIfdTag == _T("ExposureMode")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Auto exposure"); break;
				case 1 : strValOut = _T("Manual exposure"); break;
				case 2 : strValOut = _T("Auto bracket"); break;
			}
		} else if (strIfdTag == _T("WhiteBalance")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Auto white balance"); break;
				case 1 : strValOut = _T("Manual white balance"); break;
			}
		} else if (strIfdTag == _T("SceneCaptureType")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("Standard"); break;
				case 1 : strValOut = _T("Landscape"); break;
				case 2 : strValOut = _T("Portrait"); break;
				case 3 : strValOut = _T("Night scene"); break;
			}
		} else if (strIfdTag == _T("SceneType")) {
			switch (anValues[0]) {
				case 1 : strValOut = _T("A directly photographed image"); break;
			}
		} else if (strIfdTag == _T("LightSource")) {
			switch (anValues[0]) {
				case 0 : strValOut = _T("unknown"); break;
				case 1 : strValOut = _T("Daylight"); break;
				case 2 : strValOut = _T("Fluorescent"); break;
				case 3 : strValOut = _T("Tungsten (incandescent light)"); break;
				case 4 : strValOut = _T("Flash"); break;
				case 9 : strValOut = _T("Fine weather"); break;
				case 10 : strValOut = _T("Cloudy weather"); break;
				case 11 : strValOut = _T("Shade"); break;
				case 12 : strValOut = _T("Daylight fluorescent (D 5700  7100K)"); break;
				case 13 : strValOut = _T("Day white fluorescent (N 4600  5400K)"); break;
				case 14 : strValOut = _T("Cool white fluorescent (W 3900  4500K)"); break;
				case 15 : strValOut = _T("White fluorescent (WW 3200  3700K)"); break;
				case 17 : strValOut = _T("Standard light A"); break;
				case 18 : strValOut = _T("Standard light B"); break;
				case 19 : strValOut = _T("Standard light C"); break;
				case 20 : strValOut = _T("D55"); break;
				case 21 : strValOut = _T("D65"); break;
				case 22 : strValOut = _T("D75"); break;
				case 23 : strValOut = _T("D50"); break;
				case 24 : strValOut = _T("ISO studio tungsten"); break;
				case 255 : strValOut = _T("other light source"); break;
			}
		} else if (strIfdTag == _T("SubjectArea")) {
			switch (nIfdNumComps) {
				case 2 :
					// coords
					strValOut.Format(_T("Coords: Center=[%u,%u]"),
						anValues[0],anValues[1]);
					break;
				case 3 :
					// circle
					strValOut.Format(_T("Coords (Circle): Center=[%u,%u] Diameter=%u"),
						anValues[0],anValues[1],anValues[2]);
					break;
				case 4 :
					// rectangle
					strValOut.Format(_T("Coords (Rect): Center=[%u,%u] Width=%u Height=%u"),
						anValues[0],anValues[1],anValues[2],anValues[3]);
					break;
				default:
					// Leave default decode, unexpected value
					break;
			}
		}

		if (strIfdTag == _T("CFAPattern")) {
			unsigned nHorzRepeat,nVertRepeat;
			unsigned anCfaVal[16][16];
			unsigned nInd=0;
			unsigned nVal;
			CString	 strLine,strCol;
			nHorzRepeat = anValues[nInd+0]*256+anValues[nInd+1];
			nVertRepeat = anValues[nInd+2]*256+anValues[nInd+3];
			nInd+=4;
			if ((nHorzRepeat < 16) && (nVertRepeat < 16)) {
				bExtraDecode = TRUE;
				strTmp.Format(_T("    [%-36s] ="),(LPCTSTR)strIfdTag);
				m_pLog->AddLine(strTmp);
				for (unsigned nY=0;nY<nVertRepeat;nY++) {
					strLine.Format(_T("     %-36s  = [ "  ),_T(""));
					for (unsigned nX=0;nX<nHorzRepeat;nX++) {
						if (nInd<MAX_anValues) {
							nVal = anValues[nInd++];
							anCfaVal[nY][nX] = nVal;
							switch(nVal) {
								case 0: strCol = _T("Red");break;
								case 1: strCol = _T("Grn");break;
								case 2: strCol = _T("Blu");break;
								case 3: strCol = _T("Cya");break;
								case 4: strCol = _T("Mgn");break;
								case 5: strCol = _T("Yel");break;
								case 6: strCol = _T("Wht");break;
								default: strCol.Format(_T("x%02X"),nVal);break;
							}
							strLine.AppendFormat(_T("%s "),(LPCTSTR)strCol);
						}
					}
					strLine.Append(_T("]"));
					m_pLog->AddLine(strLine);
				}
			}

		}


		if ((strIfd == _T("InteropIFD")) && (strIfdTag == _T("InteroperabilityVersion"))) {
			// Assume only one
			strValOut.Format(_T("%c%c.%c%c"),anValues[0],anValues[1],anValues[2],anValues[3]);
		}


		// ==========================================================================

		// ----------------------------------------
		// Handle certain MakerNotes
		//   For Canon, we have a special parser routine to handle these
		// ----------------------------------------
		if (strIfd == _T("MakerIFD")) {

			if ((m_strImgExifMake == _T("Canon")) && (nIfdFormat == 3) && (nIfdNumComps > 4)) {
				// Print summary line now, before sub details
				// Disable later summary line
				bExtraDecode = TRUE;
				if ((!m_pAppConfig->bExifHideUnknown) || (!nIfdTagUnknown)) {
					strTmp.Format(_T("    [%-36s]"),(LPCTSTR)strIfdTag);
					m_pLog->AddLine(strTmp);

					// Assume it is a maker field with subentries!

					for (unsigned ind=0;ind<nIfdNumComps;ind++) {
						// Limit the number of entries (in case there was a decode error
						// or simply too many to report)
						if (ind<MAX_anValues) {
							strValOut.Format(_T("#%u=%u "),ind,anValues[ind]);
							strRetVal = LookupMakerCanonTag(nIfdTagVal,ind,anValues[ind]);
							strMaker = strRetVal.strTag;
							strValTmp.Format(_T("      [%-34s] = %s"),(LPCTSTR)strMaker,(LPCTSTR)(strRetVal.strVal));
							if ((!m_pAppConfig->bExifHideUnknown) || (!strRetVal.bUnknown)) {
								m_pLog->AddLine(strValTmp);
							}
						} else if (ind == MAX_anValues) {
							m_pLog->AddLine(_T("      [... etc ...]"));
						} else {
							// Don't print!
						}
					}
				}


				strValOut = _T("...");
			}

			// For Nikon & Sigma, we simply support the quality field
			if ( (strIfdTag=="Nikon1.Quality") || 
				(strIfdTag=="Nikon2.Quality") ||
				(strIfdTag=="Nikon3.Quality") ||
				(strIfdTag=="Sigma.Quality") )
			{
				m_strImgQualExif = strValOut;

				// Collect extra details (for later DB submission)
				strTmp = _T("");
				strTmp.Format(_T("[%s]:[%s],"),(LPCTSTR)strIfdTag,(LPCTSTR)strValOut);
				m_strImgExtras += strTmp;
			}

			// Collect extra details (for later DB submission)
			if (strIfdTag==_T("Canon.ImageType")) {
				strTmp = _T("");
				strTmp.Format(_T("[%s]:[%s],"),(LPCTSTR)strIfdTag,(LPCTSTR)strValOut);
				m_strImgExtras += strTmp;
			}
		}

		// ----------------------------------------

		// Now extract some of the important offsets / pointers
		if ((strIfd == _T("IFD0")) && (strIfdTag == _T("ExifOffset"))) {
			// EXIF SubIFD - Pointer
			m_nImgExifSubIfdPtr = nIfdOffset;
			strValOut.Format(_T("@ 0x%04X"),nIfdOffset);
		}

		if ((strIfd == _T("IFD0")) && (strIfdTag == _T("GPSOffset"))) {
			// GPS SubIFD - Pointer
			m_nImgExifGpsIfdPtr = nIfdOffset;
			strValOut.Format(_T("@ 0x%04X"),nIfdOffset);
		}

		// TODO: Add Interoperability IFD (0xA005)?
		if ((strIfd == _T("SubIFD")) && (strIfdTag == _T("ExifInteroperabilityOffset"))) {
			m_nImgExifInteropIfdPtr = nIfdOffset;
			strValOut.Format(_T("@ 0x%04X"),nIfdOffset);
		}

		// Extract software field
		if ((strIfd == _T("IFD0")) && (strIfdTag == _T("Software"))) {
			m_strSoftware = strValOut;
		}

		// -------------------------
		// IFD0 - ExifMake
		// -------------------------
		if ((strIfd == _T("IFD0")) && (strIfdTag == _T("Make"))) {
			m_strImgExifMake = strValOut;
			m_strImgExifMake.Trim(); // Trim whitespace (e.g. Pentax)

		}

		// -------------------------
		// IFD0 - ExifModel
		// -------------------------
		if ((strIfd == _T("IFD0")) && (strIfdTag == _T("Model"))) {
			m_strImgExifModel= strValOut;
			m_strImgExifModel.Trim();
		}


		if ((strIfd == _T("SubIFD")) && (strIfdTag == _T("MakerNote"))) {
			// Maker IFD - Pointer
			m_nImgExifMakerPtr = nIfdOffset;
			strValOut.Format(_T("@ 0x%04X"),nIfdOffset);
		}


		// -------------------------
		// IFD1 - Embedded Thumbnail
		// -------------------------
		if ((strIfd == _T("IFD1")) && (strIfdTag == _T("Compression"))) {
			// Embedded thumbnail, compression format
			m_nImgExifThumbComp = ReadSwap4(m_nPos);
		}
		if ((strIfd == _T("IFD1")) && (strIfdTag == _T("JpegIFOffset"))) {
			// Embedded thumbnail, offset
			m_nImgExifThumbOffset = nIfdOffset + nPosExifStart;
			strValOut.Format(_T("@ +0x%04X = @ 0x%04X"),nIfdOffset,m_nImgExifThumbOffset);
		}
		if ((strIfd == _T("IFD1")) && (strIfdTag == _T("JpegIFByteCount"))) {
			// Embedded thumbnail, length
			m_nImgExifThumbLen = ReadSwap4(m_nPos);
		}


		// ==========================================================================
		// Determine MakerNote support
		// ==========================================================================

		if (m_strImgExifMake != _T("")) {
			// 1) Identify the supported MakerNotes
			// 2) Remap variations of the Maker field (e.g. Nikon)
			//    as some manufacturers have been inconsistent in their
			//    use of the Make field

			m_bImgExifMakeSupported = FALSE;
			if (m_strImgExifMake == _T("Canon")) {
				m_bImgExifMakeSupported = TRUE;
			}
			else if (m_strImgExifMake == _T("PENTAX Corporation")) {
				m_strImgExifMake = _T("PENTAX");
			}
			else if (m_strImgExifMake == _T("NIKON CORPORATION")) {
				m_strImgExifMake = _T("NIKON");
				m_bImgExifMakeSupported = TRUE;
			}
			else if (m_strImgExifMake == _T("NIKON")) {
				m_strImgExifMake = _T("NIKON");
				m_bImgExifMakeSupported = TRUE;
			}
			else if (m_strImgExifMake == _T("SIGMA")) {
				m_bImgExifMakeSupported = TRUE;
			}
			else if (m_strImgExifMake == _T("SONY")) {
				m_bImgExifMakeSupported = TRUE;
			}
			else if (m_strImgExifMake == _T("FUJIFILM")) {
				// TODO:
				// FUJIFILM Maker notes apparently use
				// big endian format even though main section uses little.
				// Need to switch if in maker section for FUJI
				// For now, disable support
				m_bImgExifMakeSupported = FALSE;
			}
		}

		// Now advance the m_nPos ptr as we have finished with valoffset
		m_nPos+=4;


		// ==========================================================================
		// SUMMARY REPORT
		// ==========================================================================

		// If we haven't already output a detailed decode of this field
		// then we can output the generic representation here
		if (!bExtraDecode)
		{
			// Provide option to skip unknown fields
			if ((!m_pAppConfig->bExifHideUnknown) || (!nIfdTagUnknown)) {

				// If the tag is an ASCII string, we want to wrap with quote marks
				if (nIfdFormat == 2) {
					strTmp.Format(_T("    [%-36s] = \"%s\""),(LPCTSTR)strIfdTag,(LPCTSTR)strValOut);
				} else {
					strTmp.Format(_T("    [%-36s] = %s"),(LPCTSTR)strIfdTag,(LPCTSTR)strValOut);
				}
				m_pLog->AddLine(strTmp);

			}
		}

		DbgAddLine(_T(""));

	} // for nIfdEntryInd


	// =========== EXIF IFD (End) ===========
	// - Defined in Exif 2.2 Standard (JEITA CP-3451) section 4.6.2 
	// - Is completed by 4-byte offset to next IFD, which is
	//   read in next iteration.

	m_pLog->Enable();
	return 0;
}