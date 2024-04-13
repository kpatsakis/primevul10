ParseNodePtr Parser::ConvertToPattern(ParseNodePtr pnode)
{
    if (pnode != nullptr)
    {
        if (pnode->nop == knopArray)
        {
            ConvertArrayToArrayPattern(pnode);
        }
        else if (pnode->nop == knopObject)
        {
            pnode = ConvertObjectToObjectPattern(pnode);
        }
    }
    return pnode;
}