uint Parser::CalculateFunctionColumnNumber()
{
    uint columnNumber;

    if (m_pscan->IchMinTok() >= m_pscan->IchMinLine())
    {
        // In scenarios involving defer parse IchMinLine() can be incorrect for the first line after defer parse
        columnNumber = m_pscan->IchMinTok() - m_pscan->IchMinLine();
        if (m_functionBody != nullptr && m_functionBody->GetRelativeLineNumber() == m_pscan->LineCur())
        {
            // Adjust the column if it falls on the first line, where the re-parse is happening.
            columnNumber += m_functionBody->GetRelativeColumnNumber();
        }
    }
    else if (m_currentNodeFunc)
    {
        // For the first line after defer parse, compute the column relative to the column number
        // of the lexically parent function.
        ULONG offsetFromCurrentFunction = m_pscan->IchMinTok() - m_currentNodeFunc->ichMin;
        columnNumber = m_currentNodeFunc->sxFnc.columnNumber + offsetFromCurrentFunction ;
    }
    else
    {
        // if there is no current function, lets give a default of 0.
        columnNumber = 0;
    }

    return columnNumber;
}