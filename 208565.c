void Parser::AddModuleLocalExportEntry(ParseNodePtr varDeclNode)
{
    Assert(varDeclNode->nop == knopVarDecl || varDeclNode->nop == knopLetDecl || varDeclNode->nop == knopConstDecl);

    IdentPtr localName = varDeclNode->sxVar.pid;
    varDeclNode->sxVar.sym->SetIsModuleExportStorage(true);

    AddModuleImportOrExportEntry(EnsureModuleLocalExportEntryList(), nullptr, localName, localName, nullptr);
}