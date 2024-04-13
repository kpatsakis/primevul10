ParseNodePtr Parser::ParseCase(ParseNodePtr *ppnodeBody)
{
    ParseNodePtr pnodeT = nullptr;

    charcount_t ichMinT = m_pscan->IchMinTok();
    m_pscan->Scan();
    ParseNodePtr pnodeExpr = ParseExpr<buildAST>();
    charcount_t ichLim = m_pscan->IchLimTok();

    ChkCurTok(tkColon, ERRnoColon);

    if (buildAST)
    {
        pnodeT = CreateNodeWithScanner<knopCase>(ichMinT);
        pnodeT->sxCase.pnodeExpr = pnodeExpr;
        pnodeT->ichLim = ichLim;
    }
    ParseStmtList<buildAST>(ppnodeBody);

    return pnodeT;
}