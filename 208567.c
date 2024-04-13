ParseNodePtr Parser::CreateBlockNode(charcount_t ichMin,charcount_t ichLim, PnodeBlockType blockType)
{
    return StaticCreateBlockNode(&m_nodeAllocator, ichMin, ichLim, this->m_nextBlockId++, blockType);
}