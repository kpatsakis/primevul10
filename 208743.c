ParseNode* Parser::GetFunctionBlock()
{
    Assert(m_currentBlockInfo != nullptr);
    return m_currentBlockInfo->pBlockInfoFunction->pnodeBlock;
}