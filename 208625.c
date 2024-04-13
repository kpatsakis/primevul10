LPCOLESTR Parser::ConstructNameHint(ParseNodePtr pNode, uint32* fullNameHintLength, uint32 *pShortNameOffset)
{
    Assert(pNode != nullptr);
    Assert(pNode->nop == knopDot || pNode->nop == knopIndex);
    LPCOLESTR leftNode = nullptr;
    if (pNode->sxBin.pnode1->nop == knopDot || pNode->sxBin.pnode1->nop == knopIndex)
    {
        leftNode = ConstructNameHint(pNode->sxBin.pnode1, fullNameHintLength, pShortNameOffset);
    }
    else if (pNode->sxBin.pnode1->nop == knopName)
    {
        leftNode = pNode->sxBin.pnode1->sxPid.pid->Psz();
        *fullNameHintLength = pNode->sxBin.pnode1->sxPid.pid->Cch();
        *pShortNameOffset = 0;
    }

    if (pNode->nop == knopIndex)
    {
        return FormatPropertyString(
            leftNode ? leftNode : Js::Constants::AnonymousFunction, // e.g. f()[0] = function () {}
            pNode->sxBin.pnode2, fullNameHintLength, pShortNameOffset);
    }

    Assert(pNode->sxBin.pnode2->nop == knopDot || pNode->sxBin.pnode2->nop == knopName);

    LPCOLESTR rightNode = nullptr;
    bool wrapWithBrackets = false;
    if (pNode->sxBin.pnode2->nop == knopDot)
    {
        rightNode = ConstructNameHint(pNode->sxBin.pnode2, fullNameHintLength, pShortNameOffset);
    }
    else
    {
        rightNode = pNode->sxBin.pnode2->sxPid.pid->Psz();
        wrapWithBrackets = PNodeFlags::fpnIndexOperator == (pNode->grfpn & PNodeFlags::fpnIndexOperator);
    }
    Assert(rightNode != nullptr);
    return AppendNameHints(leftNode, rightNode, fullNameHintLength, pShortNameOffset, false, wrapWithBrackets);
}