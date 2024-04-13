bool Parser::IsStrictMode() const
{
    return (m_fUseStrictMode ||
           (m_currentNodeFunc != nullptr && m_currentNodeFunc->sxFnc.GetStrictMode()));
}