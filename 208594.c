ParseNodePtr Parser::CreateIntNodeWithScanner(int32 lw)
{
    Assert(!this->m_deferringAST);
    ParseNodePtr pnode = CreateNodeWithScanner<knopInt>();
    pnode->sxInt.lw = lw;
    return pnode;
}