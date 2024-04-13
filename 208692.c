IdentPtr Parser::ParseClassPropertyName(IdentPtr * pidHint)
{
    if (m_token.tk == tkID || m_token.tk == tkStrCon || m_token.IsReservedWord())
    {
        IdentPtr pid;
        if (m_token.tk == tkStrCon)
        {
            if (m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
            {
                Error(ERRES5NoOctal);
            }

            pid = m_token.GetStr();
        }
        else
        {
            pid = m_token.GetIdentifier(m_phtbl);
        }
        *pidHint = pid;
        return pid;
    }
    else if (m_token.tk == tkIntCon)
    {
        if (m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
        {
            Error(ERRES5NoOctal);
        }

        return m_pscan->PidFromLong(m_token.GetLong());
    }
    else if (m_token.tk == tkFltCon)
    {
        if (m_pscan->IsOctOrLeadingZeroOnLastTKNumber())
        {
            Error(ERRES5NoOctal);
        }

        return m_pscan->PidFromDbl(m_token.GetDouble());
    }

    Error(ERRnoMemberIdent);
}