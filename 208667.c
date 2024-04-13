BlockInfoStack *Parser::PushBlockInfo(ParseNodePtr pnodeBlock)
{
    BlockInfoStack *newBlockInfo = (BlockInfoStack *)m_nodeAllocator.Alloc(sizeof(BlockInfoStack));
    Assert(nullptr != newBlockInfo);

    newBlockInfo->pnodeBlock = pnodeBlock;
    newBlockInfo->pBlockInfoOuter = m_currentBlockInfo;
    newBlockInfo->m_ppnodeLex = &pnodeBlock->sxBlock.pnodeLexVars;

    if (pnodeBlock->sxBlock.blockType != PnodeBlockType::Regular)
    {
        newBlockInfo->pBlockInfoFunction = newBlockInfo;
    }
    else
    {
        Assert(m_currentBlockInfo);
        newBlockInfo->pBlockInfoFunction = m_currentBlockInfo->pBlockInfoFunction;
    }

    m_currentBlockInfo = newBlockInfo;
    return newBlockInfo;
}