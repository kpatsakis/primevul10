unsigned CjfifDecode::ReadSwap2(unsigned nPos)
{
	return ByteSwap2(Buf(nPos+0),Buf(nPos+1));
}