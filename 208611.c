ParseNodePtr Parser::StartParseBlockHelper(PnodeBlockType blockType, Scope *scope, ParseNodePtr pnodeLabel, LabelId* pLabelId)
{
    ParseNodePtr pnodeBlock = CreateBlockNode(blockType);
    pnodeBlock->sxBlock.scope = scope;
    BlockInfoStack *newBlockInfo = PushBlockInfo(pnodeBlock);

    PushStmt<buildAST>(&newBlockInfo->pstmt, pnodeBlock, knopBlock, pnodeLabel, pLabelId);

    return pnodeBlock;
}