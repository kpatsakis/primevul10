void PrintScopesWIndent(ParseNode *pnode,int indentAmt) {
    ParseNode *scope = nullptr;
    bool firstOnly = false;
    switch(pnode->nop)
    {
    case knopProg:
    case knopFncDecl: scope = pnode->sxFnc.pnodeScopes; break;
    case knopBlock: scope = pnode->sxBlock.pnodeScopes; break;
    case knopCatch: scope = pnode->sxCatch.pnodeScopes; break;
    case knopWith: scope = pnode->sxWith.pnodeScopes; break;
    case knopSwitch: scope = pnode->sxSwitch.pnodeBlock; firstOnly = true; break;
    case knopFor: scope = pnode->sxFor.pnodeBlock; firstOnly = true; break;
    case knopForIn: scope = pnode->sxForInOrForOf.pnodeBlock; firstOnly = true; break;
    case knopForOf: scope = pnode->sxForInOrForOf.pnodeBlock; firstOnly = true; break;
    }
    if (scope) {
        Output::Print(_u("[%4d, %4d): "), scope->ichMin, scope->ichLim);
        Indent(indentAmt);
        Output::Print(_u("Scopes: "));
        ParseNode *next = nullptr;
        ParseNode *syntheticBlock = nullptr;
        while (scope) {
            switch (scope->nop) {
            case knopFncDecl: Output::Print(_u("knopFncDecl")); next = scope->sxFnc.pnodeNext; break;
            case knopBlock: Output::Print(_u("knopBlock")); PrintBlockType(scope->sxBlock.blockType); next = scope->sxBlock.pnodeNext; break;
            case knopCatch: Output::Print(_u("knopCatch")); next = scope->sxCatch.pnodeNext; break;
            case knopWith: Output::Print(_u("knopWith")); next = scope->sxWith.pnodeNext; break;
            default: Output::Print(_u("unknown")); break;
            }
            if (firstOnly) {
                next = nullptr;
                syntheticBlock = scope;
            }
            if (scope->grfpn & fpnSyntheticNode) {
                Output::Print(_u(" synthetic"));
                if (scope->nop == knopBlock)
                    syntheticBlock = scope;
            }
            Output::Print(_u(" (%d-%d)"), scope->ichMin, scope->ichLim);
            if (next) Output::Print(_u(", "));
            scope = next;
        }
        Output::Print(_u("\n"));
        if (syntheticBlock || firstOnly) {
            PrintScopesWIndent(syntheticBlock, indentAmt + INDENT_SIZE);
        }
    }
}