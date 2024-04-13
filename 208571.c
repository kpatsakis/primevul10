BOOL Parser::IsConstantInFunctionCall(ParseNodePtr pnode)
{
    if (pnode->nop == knopInt && !Js::TaggedInt::IsOverflow(pnode->sxInt.lw))
    {
        return TRUE;
    }

    if (pnode->nop == knopFlt)
    {
        return TRUE;
    }

    return FALSE;
}