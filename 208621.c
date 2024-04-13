ParseNodePtr Parser::CreateIntNode(int32 lw)
{
    ParseNodePtr pnode = CreateNode(knopInt);
    pnode->sxInt.lw = lw;
    return pnode;
}