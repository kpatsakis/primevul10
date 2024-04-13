void Parser::FinishParseBlock(ParseNode *pnodeBlock, bool needScanRCurly)
{
    Assert(m_currentBlockInfo != nullptr && pnodeBlock == m_currentBlockInfo->pnodeBlock);

    if (needScanRCurly)
    {
        // Only update the ichLim if we were expecting an RCurly. If there is an
        // expression body without a necessary RCurly, the correct ichLim will
        // have been set already.
        pnodeBlock->ichLim = m_pscan->IchLimTok();
    }

    BindPidRefs<false>(GetCurrentBlockInfo(), m_nextBlockId - 1);

    PopStmt(&m_currentBlockInfo->pstmt);

    PopBlockInfo();

    Scope *scope = pnodeBlock->sxBlock.scope;
    if (scope)
    {
        PopScope(scope);
    }
}