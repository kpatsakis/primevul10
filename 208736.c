void Parser::RestoreStateFrom(ParserState *state)
{
    Assert(state != nullptr);
    Assert(state->m_currentBlockInfo == m_currentBlockInfo);

    m_funcInArray = state->m_funcInArraySave;
    m_funcInArrayDepth = state->m_funcInArrayDepthSave;
    *m_pnestedCount = state->m_nestedCountSave;
    m_pCurrentAstSize = state->m_pCurrentAstSizeSave;
    m_nextBlockId = state->m_nextBlockId;

    if (state->m_ppnodeScopeSave != nullptr)
    {
        *state->m_ppnodeScopeSave = nullptr;
    }

    if (state->m_ppnodeExprScopeSave != nullptr)
    {
        *state->m_ppnodeExprScopeSave = nullptr;
    }

    m_ppnodeScope = state->m_ppnodeScopeSave;
    m_ppnodeExprScope = state->m_ppnodeExprScopeSave;
}