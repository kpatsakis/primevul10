ParseNodePtr Parser::CreateTempNode(ParseNode* initExpr)
{
    ParseNodePtr pnode = CreateNode(knopTemp, (charcount_t)0);
    pnode->sxVar.pnodeInit =initExpr;
    pnode->sxVar.pnodeNext = nullptr;
    return pnode;
}