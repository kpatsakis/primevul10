void CjfifDecode::PrepareSignatureThumb()
{
	// Generate m_strHashThumb
	PrepareSignatureThumbSingle(false);
	// Generate m_strHashThumbRot
	PrepareSignatureThumbSingle(true);
}