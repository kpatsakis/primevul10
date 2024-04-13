bool PnBlock::HasBlockScopedContent() const
{
    // A block has its own content if a let, const, or function is declared there.

    if (this->pnodeLexVars != nullptr || this->blockType == Parameter)
    {
        return true;
    }

    // The enclosing scopes can contain functions and other things, so walk the list
    // looking specifically for functions.

    for (ParseNodePtr pnode = this->pnodeScopes; pnode;)
    {
        switch (pnode->nop) {

        case knopFncDecl:
            return true;

        case knopBlock:
            pnode = pnode->sxBlock.pnodeNext;
            break;

        case knopCatch:
            pnode = pnode->sxCatch.pnodeNext;
            break;

        case knopWith:
            pnode = pnode->sxWith.pnodeNext;
            break;

        default:
            Assert(UNREACHED);
            return true;
        }
    }

    return false;
}