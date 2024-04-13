void Parser::ParseImportClause(ModuleImportOrExportEntryList* importEntryList, bool parsingAfterComma)
{
    bool parsedNamespaceOrNamedImport = false;

    switch (m_token.tk)
    {
    case tkID:
        // This is the default binding identifier.

        // If we already saw a comma in the import clause, this is a syntax error.
        if (parsingAfterComma)
        {
            Error(ERRsyntax);
        }

        if (buildAST)
        {
            IdentPtr localName = m_token.GetIdentifier(m_phtbl);
            IdentPtr importName = wellKnownPropertyPids._default;

            CreateModuleImportDeclNode(localName);
            AddModuleImportOrExportEntry(importEntryList, importName, localName, nullptr, nullptr);
        }

        break;

    case tkLCurly:
        // This begins a list of named imports.
        ParseNamedImportOrExportClause<buildAST>(importEntryList, false);

        parsedNamespaceOrNamedImport = true;
        break;

    case tkStar:
        // This begins a namespace import clause.
        // "* as ImportedBinding"

        // Token following * must be the identifier 'as'
        m_pscan->Scan();
        if (m_token.tk != tkID || wellKnownPropertyPids.as != m_token.GetIdentifier(m_phtbl))
        {
            Error(ERRsyntax);
        }

        // Token following 'as' must be a binding identifier.
        m_pscan->Scan();
        ChkCurTokNoScan(tkID, ERRsyntax);

        if (buildAST)
        {
            IdentPtr localName = m_token.GetIdentifier(m_phtbl);
            IdentPtr importName = wellKnownPropertyPids._star;

            CreateModuleImportDeclNode(localName);
            AddModuleImportOrExportEntry(importEntryList, importName, localName, nullptr, nullptr);
        }

        parsedNamespaceOrNamedImport = true;
        break;

    default:
        Error(ERRsyntax);
    }

    m_pscan->Scan();

    if (m_token.tk == tkComma)
    {
        // There cannot be more than one comma in a module import clause.
        // There cannot be a namespace import or named imports list on the left of the comma in a module import clause.
        if (parsingAfterComma || parsedNamespaceOrNamedImport)
        {
            Error(ERRsyntax);
        }

        m_pscan->Scan();

        ParseImportClause<buildAST>(importEntryList, true);
    }
}