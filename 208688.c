BOOL Parser::IsConstantInArrayLiteral(ParseNodePtr pnode)
{
    if (pnode->nop == knopInt && !Js::TaggedInt::IsOverflow(pnode->sxInt.lw))
    {
        return TRUE;
    }
    return FALSE;
}