ParseNode* Parser::GetCurrentBlock()
{
    return m_currentBlockInfo != nullptr ? m_currentBlockInfo->pnodeBlock : nullptr;
}