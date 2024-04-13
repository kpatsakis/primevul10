void Parser::PopFuncBlockScope(ParseNodePtr *ppnodeScopeSave, ParseNodePtr *ppnodeExprScopeSave)
{
    Assert(m_ppnodeExprScope == nullptr || *m_ppnodeExprScope == nullptr);
    m_ppnodeExprScope = ppnodeExprScopeSave;

    AssertMem(m_ppnodeScope);
    Assert(nullptr == *m_ppnodeScope);
    m_ppnodeScope = ppnodeScopeSave;
}