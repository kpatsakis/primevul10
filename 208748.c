void CjfifDecode::GetDecodeSummary(CString &strHash,CString &strHashRot,CString &strImgExifMake,CString &strImgExifModel,
							CString &strImgQualExif,CString &strSoftware,teDbAdd &eDbReqSuggest)
{
	strHash = m_strHash;
	strHashRot = m_strHashRot;
	strImgExifMake = m_strImgExifMake;
	strImgExifModel = m_strImgExifModel;
	strImgQualExif = m_strImgQualExif;
	strSoftware = m_strSoftware;
	eDbReqSuggest = m_eDbReqSuggest;
}