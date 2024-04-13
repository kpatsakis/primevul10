ParseNodePtr Parser::ParseTry()
{
    ParseNodePtr pnode = nullptr;
    StmtNest stmt;
    Assert(tkTRY == m_token.tk);
    if (buildAST)
    {
        pnode = CreateNode(knopTry);
    }
    m_pscan->Scan();
    if (tkLCurly != m_token.tk)
    {
        Error(ERRnoLcurly);
    }

    PushStmt<buildAST>(&stmt, pnode, knopTry, nullptr, nullptr);
    ParseNodePtr pnodeBody = ParseStatement<buildAST>();
    if (buildAST)
    {
        pnode->sxTry.pnodeBody = pnodeBody;
        if (pnode->sxTry.pnodeBody)
            pnode->ichLim = pnode->sxTry.pnodeBody->ichLim;
    }
    PopStmt(&stmt);
    return pnode;
}