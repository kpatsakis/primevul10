unsigned CjfifDecode::DecodeIccHeader(unsigned nPos)
{
	CString strTmp,strTmp1;

	// Profile header
	unsigned nProfSz;
	unsigned nPrefCmmType;
	unsigned nProfVer;
	unsigned nProfDevClass;
	unsigned nDataColorSpace;
	unsigned nPcs;
	unsigned anDateTimeCreated[3];
	unsigned nProfFileSig;
	unsigned nPrimPlatSig;
	unsigned nProfFlags;
	unsigned nDevManuf;
	unsigned nDevModel;
	unsigned anDevAttrib[2];
	unsigned nRenderIntent;
	unsigned anIllumPcsXyz[3];
	unsigned nProfCreatorSig;
	unsigned anProfId[4];
	unsigned anRsvd[7];

	// Read in all of the ICC header bytes
	nProfSz = ReadBe4(nPos);nPos+=4;
	nPrefCmmType = ReadBe4(nPos);nPos+=4;
	nProfVer = ReadBe4(nPos);nPos+=4;
	nProfDevClass = ReadBe4(nPos);nPos+=4;
	nDataColorSpace = ReadBe4(nPos);nPos+=4;
	nPcs = ReadBe4(nPos);nPos+=4;
	anDateTimeCreated[2] = ReadBe4(nPos);nPos+=4;
	anDateTimeCreated[1] = ReadBe4(nPos);nPos+=4;
	anDateTimeCreated[0] = ReadBe4(nPos);nPos+=4;
	nProfFileSig = ReadBe4(nPos);nPos+=4;
	nPrimPlatSig = ReadBe4(nPos);nPos+=4;
	nProfFlags = ReadBe4(nPos);nPos+=4;
	nDevManuf = ReadBe4(nPos);nPos+=4;
	nDevModel = ReadBe4(nPos);nPos+=4;
	anDevAttrib[1] = ReadBe4(nPos);nPos+=4;
	anDevAttrib[0] = ReadBe4(nPos);nPos+=4;
	nRenderIntent = ReadBe4(nPos);nPos+=4;
	anIllumPcsXyz[2] = ReadBe4(nPos);nPos+=4;
	anIllumPcsXyz[1] = ReadBe4(nPos);nPos+=4;
	anIllumPcsXyz[0] = ReadBe4(nPos);nPos+=4;
	nProfCreatorSig = ReadBe4(nPos);nPos+=4;
	anProfId[3] = ReadBe4(nPos);nPos+=4;
	anProfId[2] = ReadBe4(nPos);nPos+=4;
	anProfId[1] = ReadBe4(nPos);nPos+=4;
	anProfId[0] = ReadBe4(nPos);nPos+=4;
	anRsvd[6] = ReadBe4(nPos);nPos+=4;
	anRsvd[5] = ReadBe4(nPos);nPos+=4;
	anRsvd[4] = ReadBe4(nPos);nPos+=4;
	anRsvd[3] = ReadBe4(nPos);nPos+=4;
	anRsvd[2] = ReadBe4(nPos);nPos+=4;
	anRsvd[1] = ReadBe4(nPos);nPos+=4;
	anRsvd[0] = ReadBe4(nPos);nPos+=4;

	// Now output the formatted version of the above data structures
	strTmp.Format(_T("        %-33s : %u bytes"),_T("Profile Size"),nProfSz);
	m_pLog->AddLine(strTmp);
	
	strTmp.Format(_T("        %-33s : %s"),_T("Preferred CMM Type"),(LPCTSTR)Uint2Chars(nPrefCmmType));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : %u.%u.%u.%u (0x%08X)"),_T("Profile Version"),
		((nProfVer & 0xF0000000)>>28),
		((nProfVer & 0x0F000000)>>24),
		((nProfVer & 0x00F00000)>>20),
		((nProfVer & 0x000F0000)>>16),
		nProfVer);
	m_pLog->AddLine(strTmp);

	switch (nProfDevClass) {
		case 'scnr':
			strTmp1.Format(_T("Input Device profile"));break;
		case 'mntr':
			strTmp1.Format(_T("Display Device profile"));break;
		case 'prtr':
			strTmp1.Format(_T("Output Device profile"));break;
		case 'link':
			strTmp1.Format(_T("DeviceLink Device profile"));break;
		case 'spac':
			strTmp1.Format(_T("ColorSpace Conversion profile"));break;
		case 'abst':
			strTmp1.Format(_T("Abstract profile"));break;
		case 'nmcl':
			strTmp1.Format(_T("Named colour profile"));break;
		default:
			strTmp1.Format(_T("? (0x%08X)"),nProfDevClass);
			break;
	}
	strTmp.Format(_T("        %-33s : %s (%s)"),_T("Profile Device/Class"),(LPCTSTR)strTmp1,(LPCTSTR)Uint2Chars(nProfDevClass));
	m_pLog->AddLine(strTmp);

	switch (nDataColorSpace) {
		case 'XYZ ':
			strTmp1.Format(_T("XYZData"));break;
		case 'Lab ':
			strTmp1.Format(_T("labData"));break;
		case 'Luv ':
			strTmp1.Format(_T("lubData"));break;
		case 'YCbr':
			strTmp1.Format(_T("YCbCrData"));break;
		case 'Yxy ':
			strTmp1.Format(_T("YxyData"));break;
		case 'RGB ':
			strTmp1.Format(_T("rgbData"));break;
		case 'GRAY':
			strTmp1.Format(_T("grayData"));break;
		case 'HSV ':
			strTmp1.Format(_T("hsvData"));break;
		case 'HLS ':
			strTmp1.Format(_T("hlsData"));break;
		case 'CMYK':
			strTmp1.Format(_T("cmykData"));break;
		case 'CMY ':
			strTmp1.Format(_T("cmyData"));break;
		case '2CLR':
			strTmp1.Format(_T("2colourData"));break;
		case '3CLR':
			strTmp1.Format(_T("3colourData"));break;
		case '4CLR':
			strTmp1.Format(_T("4colourData"));break;
		case '5CLR':
			strTmp1.Format(_T("5colourData"));break;
		case '6CLR':
			strTmp1.Format(_T("6colourData"));break;
		case '7CLR':
			strTmp1.Format(_T("7colourData"));break;
		case '8CLR':
			strTmp1.Format(_T("8colourData"));break;
		case '9CLR':
			strTmp1.Format(_T("9colourData"));break;
		case 'ACLR':
			strTmp1.Format(_T("10colourData"));break;
		case 'BCLR':
			strTmp1.Format(_T("11colourData"));break;
		case 'CCLR':
			strTmp1.Format(_T("12colourData"));break;
		case 'DCLR':
			strTmp1.Format(_T("13colourData"));break;
		case 'ECLR':
			strTmp1.Format(_T("14colourData"));break;
		case 'FCLR':
			strTmp1.Format(_T("15colourData"));break;
		default:
			strTmp1.Format(_T("? (0x%08X)"),nDataColorSpace);
			break;
	}
	strTmp.Format(_T("        %-33s : %s (%s)"),_T("Data Colour Space"),(LPCTSTR)strTmp1,(LPCTSTR)Uint2Chars(nDataColorSpace));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : %s"),_T("Profile connection space (PCS)"),(LPCTSTR)Uint2Chars(nPcs));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : %s"),_T("Profile creation date"),(LPCTSTR)DecodeIccDateTime(anDateTimeCreated));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : %s"),_T("Profile file signature"),(LPCTSTR)Uint2Chars(nProfFileSig));
	m_pLog->AddLine(strTmp);

	switch (nPrimPlatSig) {
		case 'APPL':
			strTmp1.Format(_T("Apple Computer, Inc."));break;
		case 'MSFT':
			strTmp1.Format(_T("Microsoft Corporation"));break;
		case 'SGI ':
			strTmp1.Format(_T("Silicon Graphics, Inc."));break;
		case 'SUNW':
			strTmp1.Format(_T("Sun Microsystems, Inc."));break;
		default:
			strTmp1.Format(_T("? (0x%08X)"),nPrimPlatSig);
			break;
	}
	strTmp.Format(_T("        %-33s : %s (%s)"),_T("Primary platform"),(LPCTSTR)strTmp1,(LPCTSTR)Uint2Chars(nPrimPlatSig));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : 0x%08X"),_T("Profile flags"),nProfFlags);
	m_pLog->AddLine(strTmp);
	strTmp1 = (TestBit(nProfFlags,0))?"Embedded profile":"Profile not embedded";
	strTmp.Format(_T("        %-35s > %s"),_T("Profile flags"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);
	strTmp1 = (TestBit(nProfFlags,1))?"Profile can be used independently of embedded":"Profile can't be used independently of embedded";
	strTmp.Format(_T("        %-35s > %s"),_T("Profile flags"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : %s"),_T("Device Manufacturer"),(LPCTSTR)Uint2Chars(nDevManuf));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : %s"),_T("Device Model"),(LPCTSTR)Uint2Chars(nDevModel));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : 0x%08X_%08X"),_T("Device attributes"),anDevAttrib[1],anDevAttrib[0]);
	m_pLog->AddLine(strTmp);
	strTmp1 = (TestBit(anDevAttrib[0],0))?"Transparency":"Reflective";
	strTmp.Format(_T("        %-35s > %s"),_T("Device attributes"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);
	strTmp1 = (TestBit(anDevAttrib[0],1))?"Matte":"Glossy";
	strTmp.Format(_T("        %-35s > %s"),_T("Device attributes"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);
	strTmp1 = (TestBit(anDevAttrib[0],2))?"Media polarity = positive":"Media polarity = negative";
	strTmp.Format(_T("        %-35s > %s"),_T("Device attributes"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);
	strTmp1 = (TestBit(anDevAttrib[0],3))?"Colour media":"Black & white media";
	strTmp.Format(_T("        %-35s > %s"),_T("Device attributes"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);

	switch(nRenderIntent) {
		case 0x00000000:	strTmp1.Format(_T("Perceptual"));break;
		case 0x00000001:	strTmp1.Format(_T("Media-Relative Colorimetric"));break;
		case 0x00000002:	strTmp1.Format(_T("Saturation"));break;
		case 0x00000003:	strTmp1.Format(_T("ICC-Absolute Colorimetric"));break;
		default:
			strTmp1.Format(_T("0x%08X"),nRenderIntent);
			break;
	}
	strTmp.Format(_T("        %-33s : %s"),_T("Rendering intent"),(LPCTSTR)strTmp1);
	m_pLog->AddLine(strTmp);

	// PCS illuminant

	strTmp.Format(_T("        %-33s : %s"),_T("Profile creator"),(LPCTSTR)Uint2Chars(nProfCreatorSig));
	m_pLog->AddLine(strTmp);

	strTmp.Format(_T("        %-33s : 0x%08X_%08X_%08X_%08X"),_T("Profile ID"),
		anProfId[3],anProfId[2],anProfId[1],anProfId[0]);
	m_pLog->AddLine(strTmp);

	return 0;
}