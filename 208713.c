void Parser::VisitFunctionsInScope(ParseNodePtr pnodeScopeList, Fn fn)
{
    ParseNodePtr pnodeScope;
    for (pnodeScope = pnodeScopeList; pnodeScope;)
    {
        switch (pnodeScope->nop)
        {
        case knopBlock:
            VisitFunctionsInScope(pnodeScope->sxBlock.pnodeScopes, fn);
            pnodeScope = pnodeScope->sxBlock.pnodeNext;
            break;

        case knopFncDecl:
            fn(pnodeScope);
            pnodeScope = pnodeScope->sxFnc.pnodeNext;
            break;

        case knopCatch:
            VisitFunctionsInScope(pnodeScope->sxCatch.pnodeScopes, fn);
            pnodeScope = pnodeScope->sxCatch.pnodeNext;
            break;

        case knopWith:
            VisitFunctionsInScope(pnodeScope->sxWith.pnodeScopes, fn);
            pnodeScope = pnodeScope->sxWith.pnodeNext;
            break;

        default:
            AssertMsg(false, "Unexpected node with scope list");
            return;
        }
    }
}