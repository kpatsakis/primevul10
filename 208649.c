void Parser::ParseNamedImportOrExportClause(ModuleImportOrExportEntryList* importOrExportEntryList, bool isExportClause)
{
    Assert(m_token.tk == tkLCurly);
    Assert(importOrExportEntryList != nullptr);

    m_pscan->Scan();

    while (m_token.tk != tkRCurly && m_token.tk != tkEOF)
    {
        tokens firstToken = m_token.tk;

        if (!(m_token.IsIdentifier() || m_token.IsReservedWord()))
        {
            Error(ERRsyntax);
        }

        IdentPtr identifierName = m_token.GetIdentifier(m_phtbl);
        IdentPtr identifierAs = identifierName;

        m_pscan->Scan();

        if (m_token.tk == tkID)
        {
            // We have the pattern "IdentifierName as"
            if (wellKnownPropertyPids.as != m_token.GetIdentifier(m_phtbl))
            {
                Error(ERRsyntax);
            }

            m_pscan->Scan();

            // If we are parsing an import statement, the token after 'as' must be a BindingIdentifier.
            if (!isExportClause)
            {
                ChkCurTokNoScan(tkID, ERRsyntax);
            }

            if (!(m_token.IsIdentifier() || m_token.IsReservedWord()))
            {
                Error(ERRsyntax);
            }

            identifierAs = m_token.GetIdentifier(m_phtbl);

            // Scan to the next token.
            m_pscan->Scan();
        }
        else if (!isExportClause && firstToken != tkID)
        {
            // If we are parsing an import statement and this ImportSpecifier clause did not have
            // 'as ImportedBinding' at the end of it, identifierName must be a BindingIdentifier.
            Error(ERRsyntax);
        }

        if (m_token.tk == tkComma)
        {
            // Consume a trailing comma
            m_pscan->Scan();
        }

        if (buildAST)
        {
            // The name we will use 'as' this import/export is a binding identifier in import statements.
            if (!isExportClause)
            {
                CreateModuleImportDeclNode(identifierAs);
                AddModuleImportOrExportEntry(importOrExportEntryList, identifierName, identifierAs, nullptr, nullptr);
            }
            else
            {
                identifierName->SetIsModuleExport();
                AddModuleImportOrExportEntry(importOrExportEntryList, nullptr, identifierName, identifierAs, nullptr);
            }
        }
    }

    // Final token in a named import or export clause must be a '}'
    ChkCurTokNoScan(tkRCurly, ERRsyntax);
}