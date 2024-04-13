ParseNodePtr Parser::CreateVarDeclNode(IdentPtr pid, SymbolType symbolType, bool autoArgumentsObject, ParseNodePtr pnodeFnc, bool errorOnRedecl, bool *isRedecl)
{
    ParseNodePtr pnode = CreateDeclNode(knopVarDecl, pid, symbolType, errorOnRedecl, isRedecl);

    // Append the variable to the end of the current variable list.
    AssertMem(m_ppnodeVar);
    pnode->sxVar.pnodeNext = *m_ppnodeVar;
    *m_ppnodeVar = pnode;
    if (nullptr != pid)
    {
        // this is not a temp - make sure temps go after this node
        AssertMem(pid);
        m_ppnodeVar = &pnode->sxVar.pnodeNext;
        CheckPidIsValid(pid, autoArgumentsObject);
    }

    return pnode;
}