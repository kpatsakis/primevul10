ParseNodePtr Parser::CreateNodeWithScanner()
{
    Assert(m_pscan != nullptr);
    return CreateNodeWithScanner<nop>(m_pscan->IchMinTok());
}