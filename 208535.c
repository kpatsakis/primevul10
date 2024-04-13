BOOL Parser::NodeIsEvalName(ParseNodePtr pnode)
{
    //WOOB 1107758 Special case of indirect eval binds to local scope in standards mode
    return pnode->nop == knopName && (pnode->sxPid.pid == wellKnownPropertyPids.eval);
}