IdentPtr Parser::PidFromNode(ParseNodePtr pnode)
{
    for (;;)
    {
        switch (pnode->nop)
        {
        case knopName:
            return pnode->sxPid.pid;

        case knopVarDecl:
            return pnode->sxVar.pid;

        case knopDot:
            Assert(pnode->sxBin.pnode2->nop == knopName);
            return pnode->sxBin.pnode2->sxPid.pid;

        case knopComma:
            // Advance to the RHS and iterate.
            pnode = pnode->sxBin.pnode2;
            break;

        default:
            return nullptr;
        }
    }
}