ParseNodePtr Parser::GetRightSideNodeFromPattern(ParseNodePtr pnode)
{
    Assert(pnode != nullptr);
    ParseNodePtr rightNode = nullptr;
    OpCode op = pnode->nop;
    if (op == knopObject)
    {
        rightNode = ConvertObjectToObjectPattern(pnode);
    }
    else if (op == knopArray)
    {
        rightNode = ConvertArrayToArrayPattern(pnode);
    }
    else
    {
        rightNode = pnode;
        if (op == knopName)
        {
            TrackAssignment<true>(pnode, nullptr);
        }
    }

    return rightNode;
}