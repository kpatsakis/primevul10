ParseNodePtr Parser::ParseFinally()
{
    ParseNodePtr pnode = nullptr;
    StmtNest stmt;
    Assert(tkFINALLY == m_token.tk);
    if (buildAST)
    {
        pnode = CreateNode(knopFinally);
    }
    m_pscan->Scan();
    if (tkLCurly != m_token.tk)
    {
        Error(ERRnoLcurly);
    }

    PushStmt<buildAST>(&stmt, pnode, knopFinally, nullptr, nullptr);
    ParseNodePtr pnodeBody = ParseStatement<buildAST>();
    if (buildAST)
    {
        pnode->sxFinally.pnodeBody = pnodeBody;
        if (!pnode->sxFinally.pnodeBody)
            // Will only occur due to error correction.
            pnode->sxFinally.pnodeBody = CreateNodeWithScanner<knopEmpty>();
        else
            pnode->ichLim = pnode->sxFinally.pnodeBody->ichLim;
    }
    PopStmt(&stmt);

    return pnode;
}