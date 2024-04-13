ParseNodePtr Parser::CreateStrNodeWithScanner(IdentPtr pid)
{
    Assert(!this->m_deferringAST);

    ParseNodePtr pnode = CreateNodeWithScanner<knopStr>();
    pnode->sxPid.pid=pid;
    pnode->grfpn |= PNodeFlags::fpnCanFlattenConcatExpr;
    return pnode;
}