void Parser::AppendToList(ParseNodePtr *node, ParseNodePtr nodeToAppend)
{
    Assert(nodeToAppend);
    ParseNodePtr* lastPtr = node;
    while ((*lastPtr) && (*lastPtr)->nop == knopList)
    {
        lastPtr = &(*lastPtr)->sxBin.pnode2;
    }
    auto last = (*lastPtr);
    if (last)
    {
        *lastPtr = CreateBinNode(knopList, last, nodeToAppend, last->ichMin, nodeToAppend->ichLim);
    }
    else
    {
        *lastPtr = nodeToAppend;
    }
}