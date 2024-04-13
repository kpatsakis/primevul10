template<bool buildAST> void Parser::ParseComputedName(ParseNodePtr* ppnodeName, LPCOLESTR* ppNameHint, LPCOLESTR* ppFullNameHint, uint32 *pNameLength, uint32 *pShortNameOffset)
{
    m_pscan->Scan();
    ParseNodePtr pnodeNameExpr = ParseExpr<buildAST>(koplCma, nullptr, TRUE, FALSE, *ppNameHint, pNameLength, pShortNameOffset);
    if (buildAST)
    {
        *ppnodeName = CreateNodeT<knopComputedName>(pnodeNameExpr->ichMin, pnodeNameExpr->ichLim);
        (*ppnodeName)->sxUni.pnode1 = pnodeNameExpr;
    }

    if (ppFullNameHint && buildAST && CONFIG_FLAG(UseFullName))
    {
        *ppFullNameHint = FormatPropertyString(*ppNameHint, pnodeNameExpr, pNameLength, pShortNameOffset);
    }

    ChkCurTokNoScan(tkRBrack, ERRnoRbrack);
}