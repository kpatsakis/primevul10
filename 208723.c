IdentPtr Parser::ParseImportOrExportFromClause(bool throwIfNotFound)
{
    IdentPtr moduleSpecifier = nullptr;

    if (m_token.tk == tkID && wellKnownPropertyPids.from == m_token.GetIdentifier(m_phtbl))
    {
        m_pscan->Scan();

        // Token following the 'from' token must be a string constant - the module specifier.
        ChkCurTokNoScan(tkStrCon, ERRsyntax);

        if (buildAST)
        {
            moduleSpecifier = m_token.GetStr();
        }

        m_pscan->Scan();
    }
    else if (throwIfNotFound)
    {
        Error(ERRsyntax);
    }

    return moduleSpecifier;
}