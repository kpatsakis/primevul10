ParseNodePtr Parser::ParseImportDeclaration()
{
    Assert(m_scriptContext->GetConfig()->IsES6ModuleEnabled());
    Assert(m_token.tk == tkIMPORT);

    if (!IsImportOrExportStatementValidHere())
    {
        Error(ERRInvalidModuleImportOrExport);
    }

    // We just parsed an import token. Next valid token is *, {, string constant, or binding identifier.
    m_pscan->Scan();

    if (m_token.tk == tkStrCon)
    {
        // This import declaration has no import clause.
        // "import ModuleSpecifier;"
        if (buildAST)
        {
            AddModuleSpecifier(m_token.GetStr());
        }

        // Scan past the module identifier.
        m_pscan->Scan();
    }
    else
    {
        ModuleImportOrExportEntryList importEntryList(&m_nodeAllocator);

        // Parse the import clause (default binding can only exist before the comma).
        ParseImportClause<buildAST>(&importEntryList);

        // Token following import clause must be the identifier 'from'
        IdentPtr moduleSpecifier = ParseImportOrExportFromClause<buildAST>(true);

        if (buildAST)
        {
            Assert(moduleSpecifier != nullptr);

            AddModuleSpecifier(moduleSpecifier);

            importEntryList.Map([this, moduleSpecifier](ModuleImportOrExportEntry& importEntry) {
                importEntry.moduleRequest = moduleSpecifier;
                AddModuleImportOrExportEntry(EnsureModuleImportEntryList(), &importEntry);
            });
        }

        importEntryList.Clear();
    }

    // Import statement is actually a nop, we hoist all the imported bindings to the top of the module.
    return nullptr;
}