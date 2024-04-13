ParseNodePtr Parser::CreateUniNode(OpCode nop, ParseNodePtr pnode1)
{
    Assert(!this->m_deferringAST);
    DebugOnly(VerifyNodeSize(nop, kcbPnUni));
    ParseNodePtr pnode = (ParseNodePtr)m_nodeAllocator.Alloc(kcbPnUni);

    Assert(m_pCurrentAstSize != nullptr);
    *m_pCurrentAstSize += kcbPnUni;

    InitNode(nop, pnode);

    pnode->sxUni.pnode1 = pnode1;
    if (nullptr == pnode1)
    {
        // no ops
        pnode->ichMin = m_pscan->IchMinTok();
        pnode->ichLim = m_pscan->IchLimTok();
    }
    else
    {
        // 1 op
        pnode->ichMin = pnode1->ichMin;
        pnode->ichLim = pnode1->ichLim;
        this->CheckArguments(pnode);
    }
    return pnode;
}