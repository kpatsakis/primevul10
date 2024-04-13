BOOL Parser::NodeEqualsName(ParseNodePtr pnode, LPCOLESTR sz, uint32 cch)
{
    return pnode->nop == knopName &&
        pnode->sxPid.pid->Cch() == cch &&
        !wmemcmp(pnode->sxPid.pid->Psz(), sz, cch);
}