BOOL Parser::IsDeferredFnc()
{
    if (m_grfscr & fscrDeferredFnc)
    {
        m_grfscr &= ~fscrDeferredFnc;
        return true;
    }

    return false;
}