void CjfifDecode::PrepareSignature()
{
	// Set m_strHash
	PrepareSignatureSingle(false);
	// Set m_strHashRot
	PrepareSignatureSingle(true);
}