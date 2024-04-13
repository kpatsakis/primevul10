BYTE CjfifDecode::Buf(unsigned long nOffset,bool bClean=false)
{
	// Buffer can be redirected to internal array for AVI DHT
	// tables, so check for it here.
	if (m_bBufFakeDHT) {
		return m_abMJPGDHTSeg[nOffset];
	} else {
		return m_pWBuf->Buf(nOffset,bClean);
	}
}