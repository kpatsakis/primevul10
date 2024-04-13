ParseNodePtr Parser::ConvertObjectToObjectPattern(ParseNodePtr pnodeMemberList)
{
    charcount_t ichMin = m_pscan->IchMinTok();
    charcount_t ichLim = m_pscan->IchLimTok();
    ParseNodePtr pnodeMemberNodeList = nullptr;
    if (pnodeMemberList != nullptr && pnodeMemberList->nop == knopObject)
    {
        ichMin = pnodeMemberList->ichMin;
        ichLim = pnodeMemberList->ichLim;
        pnodeMemberList = pnodeMemberList->sxUni.pnode1;
    }

    ForEachItemInList(pnodeMemberList, [&](ParseNodePtr item) {
        ParseNodePtr memberNode = ConvertMemberToMemberPattern(item);
        AppendToList(&pnodeMemberNodeList, memberNode);
    });

    return CreateUniNode(knopObjectPattern, pnodeMemberNodeList, ichMin, ichLim);
}