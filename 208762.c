unsigned CjfifDecode::ReadSwap4(unsigned nPos)
{
	return ByteSwap4(Buf(nPos),Buf(nPos+1),Buf(nPos+2),Buf(nPos+3));
}