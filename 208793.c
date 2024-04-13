bool CjfifDecode::DecodeValRational(unsigned nPos,float &nVal)
{
	int	nValNumer;
	int nValDenom;
	nVal = 0;

	nValNumer = ByteSwap4(Buf(nPos+0),Buf(nPos+1),Buf(nPos+2),Buf(nPos+3));
	nValDenom = ByteSwap4(Buf(nPos+4),Buf(nPos+5),Buf(nPos+6),Buf(nPos+7));

	if (nValDenom == 0) {
		// Divide by zero!
		return false;
	} else {
		nVal = (float)nValNumer/(float)nValDenom;
		return true;
	}
}