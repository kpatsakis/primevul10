ParseNodePtr Parser::CreateStrNode(IdentPtr pid)
{
    Assert(!this->m_deferringAST);

    ParseNodePtr pnode = CreateNode(knopStr);
    pnode->sxPid.pid=pid;
    pnode->grfpn |= PNodeFlags::fpnCanFlattenConcatExpr;
    return pnode;
}