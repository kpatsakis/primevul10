ParseNodePtr Parser::StartParseBlockWithCapacity(PnodeBlockType blockType, ScopeType scopeType, int capacity)
{
    Scope *scope = nullptr;
    // Block scopes are created lazily when we discover block-scoped content.
    if (scopeType != ScopeType_Unknown && scopeType != ScopeType_Block)
    {
        scope = Anew(&m_nodeAllocator, Scope, &m_nodeAllocator, scopeType, capacity);
        PushScope(scope);
    }

    return StartParseBlockHelper<buildAST>(blockType, scope, nullptr, nullptr);
}