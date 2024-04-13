void Parser::PushDynamicBlock()
{
    Assert(GetCurrentBlock());
    int blockId = GetCurrentBlock()->sxBlock.blockId;
    if (m_currentDynamicBlock && m_currentDynamicBlock->id == blockId)
    {
        return;
    }
    BlockIdsStack *info = (BlockIdsStack *)m_nodeAllocator.Alloc(sizeof(BlockIdsStack));
    if (nullptr == info)
    {
        Error(ERRnoMemory);
    }

    info->id = blockId;
    info->prev = m_currentDynamicBlock;
    m_currentDynamicBlock = info;
}