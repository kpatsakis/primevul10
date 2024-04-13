void PrintFormalsWIndent(ParseNode *pnodeArgs, int indentAmt)
{
    for (ParseNode *pnode = pnodeArgs; pnode != nullptr; pnode = pnode->GetFormalNext())
    {
        PrintPnodeWIndent(pnode->nop == knopParamPattern ? pnode->sxParamPattern.pnode1 : pnode, indentAmt);
    }
}