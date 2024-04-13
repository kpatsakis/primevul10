ParseNodePtr Parser::ParseBlock(ParseNodePtr pnodeLabel, LabelId* pLabelId)
{
    ParseNodePtr pnodeBlock = nullptr;
    ParseNodePtr *ppnodeScopeSave = nullptr;
    ParseNodePtr *ppnodeExprScopeSave = nullptr;

    pnodeBlock = StartParseBlock<buildAST>(PnodeBlockType::Regular, ScopeType_Block, pnodeLabel, pLabelId);

    BlockInfoStack* outerBlockInfo = m_currentBlockInfo->pBlockInfoOuter;
    if (outerBlockInfo != nullptr && outerBlockInfo->pnodeBlock != nullptr
        && outerBlockInfo->pnodeBlock->sxBlock.scope != nullptr
        && outerBlockInfo->pnodeBlock->sxBlock.scope->GetScopeType() == ScopeType_CatchParamPattern)
    {
        // If we are parsing the catch block then destructured params can have let declrations. Let's add them to the new block.
        for (ParseNodePtr pnode = m_currentBlockInfo->pBlockInfoOuter->pnodeBlock->sxBlock.pnodeLexVars; pnode; pnode = pnode->sxVar.pnodeNext)
        {
            PidRefStack* ref = PushPidRef(pnode->sxVar.sym->GetPid());
            ref->SetSym(pnode->sxVar.sym);
        }
    }

    ChkCurTok(tkLCurly, ERRnoLcurly);
    ParseNodePtr * ppnodeList = nullptr;
    if (buildAST)
    {
        PushFuncBlockScope(pnodeBlock, &ppnodeScopeSave, &ppnodeExprScopeSave);
        ppnodeList = &pnodeBlock->sxBlock.pnodeStmt;
    }

    ParseStmtList<buildAST>(ppnodeList);

    if (buildAST)
    {
        PopFuncBlockScope(ppnodeScopeSave, ppnodeExprScopeSave);
    }

    FinishParseBlock(pnodeBlock);

    ChkCurTok(tkRCurly, ERRnoRcurly);


    return pnodeBlock;
}