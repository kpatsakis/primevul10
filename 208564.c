int Parser::GetCurrentDynamicBlockId() const
{
    return m_currentDynamicBlock ? m_currentDynamicBlock->id : -1;
}