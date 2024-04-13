void Parser::MarkEscapingRef(ParseNodePtr pnode, IdentToken *pToken)
{
    if (m_currentNodeFunc == nullptr)
    {
        return;
    }
    if (pnode && pnode->nop == knopFncDecl)
    {
        this->SetNestedFuncEscapes();
    }
    else if (pToken->pid)
    {
        PidRefStack *pidRef = pToken->pid->GetTopRef();
        if (pidRef->sym)
        {
            if (pidRef->sym->GetSymbolType() == STFunction)
            {
                this->SetNestedFuncEscapes();
            }
        }
        else
        {
            pidRef->isEscape = true;
        }
    }
}