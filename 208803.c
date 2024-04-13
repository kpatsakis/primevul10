CString CjfifDecode::DecodeValFraction(unsigned nPos)
{
	CString strTmp;
	int nValNumer = ReadSwap4(nPos+0);
	int nValDenom = ReadSwap4(nPos+4);
	strTmp.Format(_T("%d/%d"),nValNumer,nValDenom);
	return strTmp;
}