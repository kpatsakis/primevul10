void CjfifDecode::AddHeader(unsigned nCode)
{
	CString strTmp;

	switch(nCode)
	{
	case JFIF_SOI: m_pLog->AddLineHdr(_T("*** Marker: SOI (xFFD8) ***")); break;

	case JFIF_APP0: m_pLog->AddLineHdr(_T("*** Marker: APP0 (xFFE0) ***")); break;
	case JFIF_APP1: m_pLog->AddLineHdr(_T("*** Marker: APP1 (xFFE1) ***")); break;
	case JFIF_APP2: m_pLog->AddLineHdr(_T("*** Marker: APP2 (xFFE2) ***")); break;
	case JFIF_APP3: m_pLog->AddLineHdr(_T("*** Marker: APP3 (xFFE3) ***")); break;
	case JFIF_APP4: m_pLog->AddLineHdr(_T("*** Marker: APP4 (xFFE4) ***")); break;
	case JFIF_APP5: m_pLog->AddLineHdr(_T("*** Marker: APP5 (xFFE5) ***")); break;
	case JFIF_APP6: m_pLog->AddLineHdr(_T("*** Marker: APP6 (xFFE6) ***")); break;
	case JFIF_APP7: m_pLog->AddLineHdr(_T("*** Marker: APP7 (xFFE7) ***")); break;
	case JFIF_APP8: m_pLog->AddLineHdr(_T("*** Marker: APP8 (xFFE8) ***")); break;
	case JFIF_APP9: m_pLog->AddLineHdr(_T("*** Marker: APP9 (xFFE9) ***")); break;
	case JFIF_APP10: m_pLog->AddLineHdr(_T("*** Marker: APP10 (xFFEA) ***")); break;
	case JFIF_APP11: m_pLog->AddLineHdr(_T("*** Marker: APP11 (xFFEB) ***")); break;
	case JFIF_APP12: m_pLog->AddLineHdr(_T("*** Marker: APP12 (xFFEC) ***")); break;
	case JFIF_APP13: m_pLog->AddLineHdr(_T("*** Marker: APP13 (xFFED) ***")); break;
	case JFIF_APP14: m_pLog->AddLineHdr(_T("*** Marker: APP14 (xFFEE) ***")); break;
	case JFIF_APP15: m_pLog->AddLineHdr(_T("*** Marker: APP15 (xFFEF) ***")); break;

	case JFIF_SOF0:  m_pLog->AddLineHdr(_T("*** Marker: SOF0 (Baseline DCT) (xFFC0) ***")); break;
	case JFIF_SOF1:  m_pLog->AddLineHdr(_T("*** Marker: SOF1 (Extended Sequential DCT, Huffman) (xFFC1) ***")); break;
	case JFIF_SOF2:  m_pLog->AddLineHdr(_T("*** Marker: SOF2 (Progressive DCT, Huffman) (xFFC2) ***")); break;
	case JFIF_SOF3:  m_pLog->AddLineHdr(_T("*** Marker: SOF3 (Lossless Process, Huffman) (xFFC3) ***")); break;
	case JFIF_SOF5:  m_pLog->AddLineHdr(_T("*** Marker: SOF5 (Differential Sequential DCT, Huffman) (xFFC4) ***")); break;
	case JFIF_SOF6:  m_pLog->AddLineHdr(_T("*** Marker: SOF6 (Differential Progressive DCT, Huffman) (xFFC5) ***")); break;
	case JFIF_SOF7:  m_pLog->AddLineHdr(_T("*** Marker: SOF7 (Differential Lossless Process, Huffman) (xFFC6) ***")); break;
	case JFIF_SOF9:  m_pLog->AddLineHdr(_T("*** Marker: SOF9 (Sequential DCT, Arithmetic) (xFFC9) ***")); break;
	case JFIF_SOF10: m_pLog->AddLineHdr(_T("*** Marker: SOF10 (Progressive DCT, Arithmetic) (xFFCA) ***")); break;
	case JFIF_SOF11: m_pLog->AddLineHdr(_T("*** Marker: SOF11 (Lossless Process, Arithmetic) (xFFCB) ***")); break;
	case JFIF_SOF13: m_pLog->AddLineHdr(_T("*** Marker: SOF13 (Differential Sequential, Arithmetic) (xFFCD) ***")); break;
	case JFIF_SOF14: m_pLog->AddLineHdr(_T("*** Marker: SOF14 (Differential Progressive DCT, Arithmetic) (xFFCE) ***")); break;
	case JFIF_SOF15: m_pLog->AddLineHdr(_T("*** Marker: SOF15 (Differential Lossless Process, Arithmetic) (xFFCF) ***")); break;

	case JFIF_JPG:   m_pLog->AddLineHdr(_T("*** Marker: JPG (xFFC8) ***")); break;
	case JFIF_DAC:   m_pLog->AddLineHdr(_T("*** Marker: DAC (xFFCC) ***")); break;

	case JFIF_RST0:
	case JFIF_RST1:
	case JFIF_RST2:
	case JFIF_RST3:
	case JFIF_RST4:
	case JFIF_RST5:
	case JFIF_RST6:
	case JFIF_RST7:
		m_pLog->AddLineHdr(_T("*** Marker: RST# ***"));
		break;

	case JFIF_DQT:  // Define quantization tables
		m_pLog->AddLineHdr(_T("*** Marker: DQT (xFFDB) ***"));
		m_pLog->AddLineHdrDesc(_T("  Define a Quantization Table."));
		break;


	case JFIF_COM: // COM
		m_pLog->AddLineHdr(_T("*** Marker: COM (Comment) (xFFFE) ***"));
		break;

	case JFIF_DHT: // DHT
		m_pLog->AddLineHdr(_T("*** Marker: DHT (Define Huffman Table) (xFFC4) ***"));
		break;
	case JFIF_DHT_FAKE: // DHT from standard table (MotionJPEG)
		m_pLog->AddLineHdr(_T("*** Marker: DHT from MotionJPEG standard (Define Huffman Table) ***"));
		break;

	case JFIF_SOS: // SOS
		m_pLog->AddLineHdr(_T("*** Marker: SOS (Start of Scan) (xFFDA) ***"));
		break;

	case JFIF_DRI: // DRI
		m_pLog->AddLineHdr(_T("*** Marker: DRI (Restart Interval) (xFFDD) ***"));
		break;

	case JFIF_EOI: // EOI
		m_pLog->AddLineHdr(_T("*** Marker: EOI (End of Image) (xFFD9) ***"));
		break;

	case JFIF_DNL:   m_pLog->AddLineHdr(_T("*** Marker: DNL (Define Number of Lines) (xFFDC) ***")); break;
	case JFIF_DHP:   m_pLog->AddLineHdr(_T("*** Marker: DHP (Define Hierarchical Progression) (xFFDE) ***")); break;
	case JFIF_EXP:   m_pLog->AddLineHdr(_T("*** Marker: EXP (Expand Reference Components) (xFFDF) ***")); break;
	case JFIF_JPG0:  m_pLog->AddLineHdr(_T("*** Marker: JPG0 (JPEG Extension) (xFFF0) ***")); break;
	case JFIF_JPG1:  m_pLog->AddLineHdr(_T("*** Marker: JPG1 (JPEG Extension) (xFFF1) ***")); break;
	case JFIF_JPG2:  m_pLog->AddLineHdr(_T("*** Marker: JPG2 (JPEG Extension) (xFFF2) ***")); break;
	case JFIF_JPG3:  m_pLog->AddLineHdr(_T("*** Marker: JPG3 (JPEG Extension) (xFFF3) ***")); break;
	case JFIF_JPG4:  m_pLog->AddLineHdr(_T("*** Marker: JPG4 (JPEG Extension) (xFFF4) ***")); break;
	case JFIF_JPG5:  m_pLog->AddLineHdr(_T("*** Marker: JPG5 (JPEG Extension) (xFFF5) ***")); break;
	case JFIF_JPG6:  m_pLog->AddLineHdr(_T("*** Marker: JPG6 (JPEG Extension) (xFFF6) ***")); break;
	case JFIF_JPG7:  m_pLog->AddLineHdr(_T("*** Marker: JPG7 (JPEG Extension) (xFFF7) ***")); break;
	case JFIF_JPG8:  m_pLog->AddLineHdr(_T("*** Marker: JPG8 (JPEG Extension) (xFFF8) ***")); break;
	case JFIF_JPG9:  m_pLog->AddLineHdr(_T("*** Marker: JPG9 (JPEG Extension) (xFFF9) ***")); break;
	case JFIF_JPG10: m_pLog->AddLineHdr(_T("*** Marker: JPG10 (JPEG Extension) (xFFFA) ***")); break;
	case JFIF_JPG11: m_pLog->AddLineHdr(_T("*** Marker: JPG11 (JPEG Extension) (xFFFB) ***")); break;
	case JFIF_JPG12: m_pLog->AddLineHdr(_T("*** Marker: JPG12 (JPEG Extension) (xFFFC) ***")); break;
	case JFIF_JPG13: m_pLog->AddLineHdr(_T("*** Marker: JPG13 (JPEG Extension) (xFFFD) ***")); break;
	case JFIF_TEM:   m_pLog->AddLineHdr(_T("*** Marker: TEM (Temporary) (xFF01) ***")); break;



	default:
		strTmp.Format(_T("*** Marker: ??? (Unknown) (xFF%02X) ***"),nCode);
		m_pLog->AddLineHdr(strTmp);
		break;
	}
	// Adjust position to account for the word used in decoding the marker!
	strTmp.Format(_T("  OFFSET: 0x%08X"),m_nPos-2);
	m_pLog->AddLine(strTmp);
}