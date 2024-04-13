bool Parser::CheckForDirective(bool* pIsUseStrict, bool *pIsUseAsm, bool* pIsOctalInString)
{
    // A directive is a string constant followed by a statement terminating token
    if (m_token.tk != tkStrCon)
        return false;

    // Careful, need to check for octal before calling m_pscan->Scan()
    // because Scan() clears the "had octal" flag on the scanner and
    // m_pscan->Restore() does not restore this flag.
    if (pIsOctalInString != nullptr)
    {
        *pIsOctalInString = m_pscan->IsOctOrLeadingZeroOnLastTKNumber();
    }

    Ident* pidDirective = m_token.GetStr();
    RestorePoint start;
    m_pscan->Capture(&start);
    m_pscan->Scan();

    bool isDirective = true;

    switch (m_token.tk)
    {
    case tkSColon:
    case tkEOF:
    case tkLCurly:
    case tkRCurly:
        break;
    default:
        if (!m_pscan->FHadNewLine())
        {
            isDirective = false;
        }
        break;
    }

    if (isDirective)
    {
        if (pIsUseStrict != nullptr)
        {
            *pIsUseStrict = CheckStrictModeStrPid(pidDirective);
        }
        if (pIsUseAsm != nullptr)
        {
            *pIsUseAsm = CheckAsmjsModeStrPid(pidDirective);
        }
    }

    m_pscan->SeekTo(start);
    return isDirective;
}