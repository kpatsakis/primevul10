ParseNodePtr Parser::StartParseBlock(PnodeBlockType blockType, ScopeType scopeType, ParseNodePtr pnodeLabel, LabelId* pLabelId)
{
    Scope *scope = nullptr;
    // Block scopes are created lazily when we discover block-scoped content.
    if (scopeType != ScopeType_Unknown && scopeType != ScopeType_Block)
    {
        scope = Anew(&m_nodeAllocator, Scope, &m_nodeAllocator, scopeType);
        PushScope(scope);
    }

    return StartParseBlockHelper<buildAST>(blockType, scope, pnodeLabel, pLabelId);
}