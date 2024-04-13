CjfifDecode::~CjfifDecode()
{
	// Free the Photoshop decoder
	if (m_pPsDec) {
		delete m_pPsDec;
		m_pPsDec = NULL;
	}

#ifdef SUPPORT_DICOM
	// Free the DICOM decoder
	if (m_pDecDicom) {
		delete m_pDecDicom;
		m_pDecDicom = NULL;
	}
#endif

}