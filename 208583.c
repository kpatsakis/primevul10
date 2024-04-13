ParseNodePtr Parser::CreateNode(OpCode nop, charcount_t ichMin)
{
    bool nodeAllowed = IsNodeAllowedInCurrentDeferralState(nop);
    Assert(nodeAllowed);

    Assert(nop >= 0 && nop < knopLim);
    ParseNodePtr pnode;
    int cb = (nop >= knopNone && nop < knopLim) ? g_mpnopcbNode[nop] : g_mpnopcbNode[knopEmpty];

    pnode = (ParseNodePtr)m_nodeAllocator.Alloc(cb);
    Assert(pnode != nullptr);

    if (!m_deferringAST)
    {
        Assert(m_pCurrentAstSize != nullptr);
        *m_pCurrentAstSize += cb;
    }

    InitNode(nop,pnode);

    // default - may be changed
    pnode->ichMin = ichMin;
    if (m_pscan!= nullptr) {
      pnode->ichLim = m_pscan->IchLimTok();
    }
    else pnode->ichLim=0;

    return pnode;
}