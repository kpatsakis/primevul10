ParseNodePtr Parser::CreateNodeWithScanner(charcount_t ichMin)
{
    Assert(m_pscan != nullptr);
    return CreateNodeT<nop>(ichMin, m_pscan->IchLimTok());
}