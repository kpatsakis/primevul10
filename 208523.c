void Parser::DeferOrEmitPotentialSpreadError(ParseNodePtr pnodeT)
{
    if (m_parenDepth > 0)
    {
        if (m_token.tk == tkRParen)
        {
           if (!m_deferEllipsisError)
            {
                // Capture only the first error instance.
                m_pscan->Capture(&m_EllipsisErrLoc);
                m_deferEllipsisError = true;
            }
        }
        else
        {
            Error(ERRUnexpectedEllipsis);
        }
    }
    else
    {
        Error(ERRInvalidSpreadUse);
    }
}