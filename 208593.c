Parser::TokIsForInOrForOf()
{
    return m_token.tk == tkIN ||
        (m_token.tk == tkID &&
         m_token.GetIdentifier(m_phtbl) == wellKnownPropertyPids.of);
}