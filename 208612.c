void Parser::PopDynamicBlock()
{
    int blockId = GetCurrentDynamicBlockId();
    if (GetCurrentBlock()->sxBlock.blockId != blockId || blockId == -1)
    {
        return;
    }
    Assert(m_currentDynamicBlock);
    for (BlockInfoStack *blockInfo = m_currentBlockInfo; blockInfo; blockInfo = blockInfo->pBlockInfoOuter)
    {
        for (ParseNodePtr pnodeDecl = blockInfo->pnodeBlock->sxBlock.pnodeLexVars;
             pnodeDecl;
             pnodeDecl = pnodeDecl->sxVar.pnodeNext)
        {
            this->SetPidRefsInScopeDynamic(pnodeDecl->sxVar.pid, blockId);
        }
    }

    m_currentDynamicBlock = m_currentDynamicBlock->prev;
}