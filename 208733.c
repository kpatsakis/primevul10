void Parser::ParseDestructuredLiteralWithScopeSave(tokens declarationType,
    bool isDecl,
    bool topLevel,
    DestructuringInitializerContext initializerContext/* = DIC_None*/,
    bool allowIn /*= true*/)
{
    // We are going to parse the text again to validate the current grammar as Destructuring. Saving some scopes and
    // AST related information before the validation parsing and later they will be restored.

    ParseNodePtr pnodeFncSave = m_currentNodeFunc;
    ParseNodePtr pnodeDeferredFncSave = m_currentNodeDeferredFunc;
    if (m_currentNodeDeferredFunc == nullptr)
    {
        m_currentNodeDeferredFunc = m_currentNodeFunc;
    }
    int32 *pAstSizeSave = m_pCurrentAstSize;
    uint *pNestedCountSave = m_pnestedCount;
    ParseNodePtr *ppnodeScopeSave = m_ppnodeScope;
    ParseNodePtr *ppnodeExprScopeSave = m_ppnodeExprScope;

    ParseNodePtr newTempScope = nullptr;
    m_ppnodeScope = &newTempScope;

    int32 newTempAstSize = 0;
    m_pCurrentAstSize = &newTempAstSize;

    uint newTempNestedCount = 0;
    m_pnestedCount = &newTempNestedCount;

    m_ppnodeExprScope = nullptr;

    charcount_t funcInArraySave = m_funcInArray;
    uint funcInArrayDepthSave = m_funcInArrayDepth;

    // we need to reset this as we are going to parse the grammar again.
    m_hasDeferredShorthandInitError = false;

    ParseDestructuredLiteral<false>(declarationType, isDecl, topLevel, initializerContext, allowIn);

    m_currentNodeFunc = pnodeFncSave;
    m_currentNodeDeferredFunc = pnodeDeferredFncSave;
    m_pCurrentAstSize = pAstSizeSave;
    m_pnestedCount = pNestedCountSave;
    m_ppnodeScope = ppnodeScopeSave;
    m_ppnodeExprScope = ppnodeExprScopeSave;
    m_funcInArray = funcInArraySave;
    m_funcInArrayDepth = funcInArrayDepthSave;
}