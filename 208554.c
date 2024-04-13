ParseNodePtr Parser::ConvertArrayToArrayPattern(ParseNodePtr pnode)
{
    Assert(pnode->nop == knopArray);
    pnode->nop = knopArrayPattern;

    ForEachItemRefInList(&pnode->sxArrLit.pnode1, [&](ParseNodePtr *itemRef) {
        ParseNodePtr item = *itemRef;
        if (item->nop == knopEllipsis)
        {
            itemRef = &item->sxUni.pnode1;
            item = *itemRef;
            if (!(item->nop == knopName
                  || item->nop == knopDot
                  || item->nop == knopIndex
                  || item->nop == knopArray
                  || item->nop == knopObject))
            {
                Error(ERRInvalidAssignmentTarget);
            }
        }
        else if (item->nop == knopAsg)
        {
            itemRef = &item->sxBin.pnode1;
            item = *itemRef;
        }

        if (item->nop == knopArray)
        {
            ConvertArrayToArrayPattern(item);
        }
        else if (item->nop == knopObject)
        {
            *itemRef = ConvertObjectToObjectPattern(item);
        }
        else if (item->nop == knopName)
        {
            TrackAssignment<true>(item, nullptr);
        }
    });

    return pnode;
}