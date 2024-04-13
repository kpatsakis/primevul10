void Parser::CaptureState(ParserState *state)
{
    Assert(state != nullptr);

    state->m_funcInArraySave = m_funcInArray;
    state->m_funcInArrayDepthSave = m_funcInArrayDepth;
    state->m_nestedCountSave = *m_pnestedCount;
    state->m_ppnodeScopeSave = m_ppnodeScope;
    state->m_ppnodeExprScopeSave = m_ppnodeExprScope;
    state->m_pCurrentAstSizeSave = m_pCurrentAstSize;
    state->m_nextBlockId = m_nextBlockId;

    Assert(state->m_ppnodeScopeSave == nullptr || *state->m_ppnodeScopeSave == nullptr);
    Assert(state->m_ppnodeExprScopeSave == nullptr || *state->m_ppnodeExprScopeSave == nullptr);

#if DEBUG
    state->m_currentBlockInfo = m_currentBlockInfo;
#endif
}