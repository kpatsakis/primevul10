ParseNodePtr Parser::CreateDeclNode(OpCode nop, IdentPtr pid, SymbolType symbolType, bool errorOnRedecl, bool *isRedecl)
{
    ParseNodePtr pnode = CreateNode(nop);

    pnode->sxVar.InitDeclNode(pid, NULL);

    if (symbolType != STUnknown)
    {
        pnode->sxVar.sym = AddDeclForPid(pnode, pid, symbolType, errorOnRedecl, isRedecl);
    }

    return pnode;
}