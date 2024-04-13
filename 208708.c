void Parser::FinishParseFncExprScope(ParseNodePtr pnodeFnc, ParseNodePtr pnodeFncExprScope)
{
    int fncExprScopeId = pnodeFncExprScope->sxBlock.blockId;
    ParseNodePtr pnodeName = pnodeFnc->sxFnc.pnodeName;
    if (pnodeName)
    {
        Assert(pnodeName->nop == knopVarDecl);
        BindPidRefsInScope(pnodeName->sxVar.pid, pnodeName->sxVar.sym, fncExprScopeId);
    }
    FinishParseBlock(pnodeFncExprScope);
}