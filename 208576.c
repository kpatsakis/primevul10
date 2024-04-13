void Parser::PopBlockInfo()
{
    Assert(m_currentBlockInfo);
    PopDynamicBlock();
    m_currentBlockInfo = m_currentBlockInfo->pBlockInfoOuter;
}