ParseNodePtr Parser::ParseTryCatchFinally()
{
    this->m_tryCatchOrFinallyDepth++;

    ParseNodePtr pnodeT = ParseTry<buildAST>();
    ParseNodePtr pnodeTC = nullptr;
    StmtNest stmt;
    bool hasCatch = false;

    if (tkCATCH == m_token.tk)
    {
        hasCatch = true;
        if (buildAST)
        {
            pnodeTC = CreateNodeWithScanner<knopTryCatch>();
            pnodeT->sxStmt.pnodeOuter = pnodeTC;
            pnodeTC->sxTryCatch.pnodeTry = pnodeT;
        }
        PushStmt<buildAST>(&stmt, pnodeTC, knopTryCatch, nullptr, nullptr);

        ParseNodePtr pnodeCatch = ParseCatch<buildAST>();
        if (buildAST)
        {
            pnodeTC->sxTryCatch.pnodeCatch = pnodeCatch;
        }
        PopStmt(&stmt);
    }
    if (tkFINALLY != m_token.tk)
    {
        if (!hasCatch)
        {
            Error(ERRnoCatch);
        }
        Assert(!buildAST || pnodeTC);
        return pnodeTC;
    }

    ParseNodePtr pnodeTF = nullptr;
    if (buildAST)
    {
        pnodeTF = CreateNode(knopTryFinally);
    }
    PushStmt<buildAST>(&stmt, pnodeTF, knopTryFinally, nullptr, nullptr);
    ParseNodePtr pnodeFinally = ParseFinally<buildAST>();
    if (buildAST)
    {
        if (!hasCatch)
        {
            pnodeTF->sxTryFinally.pnodeTry = pnodeT;
            pnodeT->sxStmt.pnodeOuter = pnodeTF;
        }
        else
        {
            pnodeTF->sxTryFinally.pnodeTry = CreateNode(knopTry);
            pnodeTF->sxTryFinally.pnodeTry->sxStmt.pnodeOuter = pnodeTF;
            pnodeTF->sxTryFinally.pnodeTry->sxTry.pnodeBody = pnodeTC;
            pnodeTC->sxStmt.pnodeOuter = pnodeTF->sxTryFinally.pnodeTry;
        }
        pnodeTF->sxTryFinally.pnodeFinally = pnodeFinally;
    }
    PopStmt(&stmt);
    this->m_tryCatchOrFinallyDepth--;
    return pnodeTF;
}