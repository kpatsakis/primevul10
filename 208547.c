void PrintPnodeListWIndent(ParseNode *pnode,int indentAmt) {
    if (pnode!=NULL) {
        while(pnode->nop==knopList) {
            PrintPnodeWIndent(pnode->sxBin.pnode1,indentAmt);
            pnode = pnode->sxBin.pnode2;
        }
        PrintPnodeWIndent(pnode,indentAmt);
    }
}