void Parser::CheckArguments(ParseNodePtr pnode)
{
    if (m_currentNodeFunc && this->NodeIsIdent(pnode, wellKnownPropertyPids.arguments))
    {
        m_currentNodeFunc->sxFnc.SetHasHeapArguments();
    }
}