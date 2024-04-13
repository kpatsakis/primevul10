ParseNodePtr Parser::CreateTriNode(OpCode nop, ParseNodePtr pnode1,
                                   ParseNodePtr pnode2, ParseNodePtr pnode3,
                                   charcount_t ichMin,charcount_t ichLim)
{
    Assert(!this->m_deferringAST);
    DebugOnly(VerifyNodeSize(nop, kcbPnTri));
    ParseNodePtr pnode = (ParseNodePtr)m_nodeAllocator.Alloc(kcbPnTri);

    Assert(m_pCurrentAstSize != NULL);
    *m_pCurrentAstSize += kcbPnTri;

    InitNode(nop, pnode);

    pnode->sxTri.pnodeNext = NULL;
    pnode->sxTri.pnode1 = pnode1;
    pnode->sxTri.pnode2 = pnode2;
    pnode->sxTri.pnode3 = pnode3;

    pnode->ichMin = ichMin;
    pnode->ichLim = ichLim;

    return pnode;
}