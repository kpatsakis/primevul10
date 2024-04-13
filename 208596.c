void Parser::PushFuncBlockScope(ParseNodePtr pnodeBlock, ParseNodePtr **ppnodeScopeSave, ParseNodePtr **ppnodeExprScopeSave)
{
    // Maintain the scope tree.

    pnodeBlock->sxBlock.pnodeScopes = nullptr;
    pnodeBlock->sxBlock.pnodeNext = nullptr;

    // Insert this block into the active list of scopes (m_ppnodeExprScope or m_ppnodeScope).
    // Save the current block's "next" pointer as the new endpoint of that list.
    if (m_ppnodeExprScope)
    {
        *ppnodeScopeSave = m_ppnodeScope;

        Assert(*m_ppnodeExprScope == nullptr);
        *m_ppnodeExprScope = pnodeBlock;
        *ppnodeExprScopeSave = &pnodeBlock->sxBlock.pnodeNext;
    }
    else
    {
        Assert(m_ppnodeScope);
        Assert(*m_ppnodeScope == nullptr);
        *m_ppnodeScope = pnodeBlock;
        *ppnodeScopeSave = &pnodeBlock->sxBlock.pnodeNext;

        *ppnodeExprScopeSave = m_ppnodeExprScope;
    }

    // Advance the global scope list pointer to the new block's child list.
    m_ppnodeScope = &pnodeBlock->sxBlock.pnodeScopes;
    // Set m_ppnodeExprScope to NULL to make that list inactive.
    m_ppnodeExprScope = nullptr;
}