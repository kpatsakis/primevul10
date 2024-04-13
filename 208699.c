void Parser::AddVarDeclToBlock(ParseNode *pnode)
{
    Assert(pnode->nop == knopConstDecl || pnode->nop == knopLetDecl);

    // Maintain a combined list of let and const declarations to keep
    // track of declaration order.

    AssertMem(m_currentBlockInfo->m_ppnodeLex);
    *m_currentBlockInfo->m_ppnodeLex = pnode;
    m_currentBlockInfo->m_ppnodeLex = &pnode->sxVar.pnodeNext;
    pnode->sxVar.pnodeNext = nullptr;
}