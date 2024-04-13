void Parser::AppendFunctionToScopeList(bool fDeclaration, ParseNodePtr pnodeFnc)
{
    if (!fDeclaration && m_ppnodeExprScope)
    {
        // We're tracking function expressions separately from declarations in this scope
        // (e.g., inside a catch scope in standards mode).
        Assert(*m_ppnodeExprScope == nullptr);
        *m_ppnodeExprScope = pnodeFnc;
        m_ppnodeExprScope = &pnodeFnc->sxFnc.pnodeNext;
    }
    else
    {
        Assert(*m_ppnodeScope == nullptr);
        *m_ppnodeScope = pnodeFnc;
        m_ppnodeScope = &pnodeFnc->sxFnc.pnodeNext;
    }
}