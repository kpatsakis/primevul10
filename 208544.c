ParseNodePtr Parser::ParseDestructuredObjectLiteral(tokens declarationType, bool isDecl, bool topLevel/* = true*/)
{
    Assert(m_token.tk == tkLCurly);
    charcount_t ichMin = m_pscan->IchMinTok();
    m_pscan->Scan();

    if (!isDecl)
    {
        declarationType = tkLCurly;
    }
    ParseNodePtr pnodeMemberList = ParseMemberList<buildAST>(nullptr/*pNameHint*/, nullptr/*pHintLength*/, declarationType);
    Assert(m_token.tk == tkRCurly);

    ParseNodePtr objectPatternNode = nullptr;
    if (buildAST)
    {
        charcount_t ichLim = m_pscan->IchLimTok();
        objectPatternNode = CreateUniNode(knopObjectPattern, pnodeMemberList, ichMin, ichLim);
    }
    return objectPatternNode;
}