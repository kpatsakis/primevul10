ParseNodePtr Parser::ConvertMemberToMemberPattern(ParseNodePtr pnodeMember)
{
    if (pnodeMember->nop == knopObjectPatternMember)
    {
        return pnodeMember;
    }

    Assert(pnodeMember->nop == knopMember || pnodeMember->nop == knopMemberShort);

    ParseNodePtr rightNode = GetRightSideNodeFromPattern(pnodeMember->sxBin.pnode2);
    ParseNodePtr resultNode = CreateBinNode(knopObjectPatternMember, pnodeMember->sxBin.pnode1, rightNode);
    resultNode->ichMin = pnodeMember->ichMin;
    resultNode->ichLim = pnodeMember->ichLim;
    return resultNode;
}