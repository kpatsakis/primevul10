BOOL Parser::NodeIsIdent(ParseNodePtr pnode, IdentPtr pid)
{
    for (;;)
    {
        switch (pnode->nop)
        {
        case knopName:
            return (pnode->sxPid.pid == pid);

        case knopComma:
            pnode = pnode->sxBin.pnode2;
            break;

        default:
            return FALSE;
        }
    }
}