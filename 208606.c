ParseNodePtr Parser::CreateNode(OpCode nop, charcount_t ichMin, charcount_t ichLim)
{
    Assert(!this->m_deferringAST);
    Assert(nop >= 0 && nop < knopLim);
    ParseNodePtr pnode;
    __analysis_assume(nop < knopLim);
    int cb = nop >= 0 && nop < knopLim ? g_mpnopcbNode[nop] : kcbPnNone;

    pnode = (ParseNodePtr)m_nodeAllocator.Alloc(cb);
    Assert(pnode);

    Assert(m_pCurrentAstSize != NULL);
    *m_pCurrentAstSize += cb;

    InitNode(nop,pnode);

    pnode->ichMin = ichMin;
    pnode->ichLim = ichLim;

    return pnode;
}