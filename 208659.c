DeferredFunctionStub * BuildDeferredStubTree(ParseNode *pnodeFnc, Recycler *recycler)
{
    Assert(pnodeFnc->nop == knopFncDecl);

    uint nestedCount = pnodeFnc->sxFnc.nestedCount;
    if (nestedCount == 0)
    {
        return nullptr;
    }

    if (pnodeFnc->sxFnc.deferredStub)
    {
        return pnodeFnc->sxFnc.deferredStub;
    }

    DeferredFunctionStub *deferredStubs = RecyclerNewArray(recycler, DeferredFunctionStub, nestedCount);
    uint i = 0;

    ParseNode *pnodeBlock = pnodeFnc->sxFnc.pnodeBodyScope;
    Assert(pnodeBlock != nullptr
        && pnodeBlock->nop == knopBlock
        && (pnodeBlock->sxBlock.blockType == PnodeBlockType::Function
            || pnodeBlock->sxBlock.blockType == PnodeBlockType::Parameter));

    for (ParseNode *pnodeChild = pnodeBlock->sxBlock.pnodeScopes; pnodeChild != nullptr;)
    {

        if (pnodeChild->nop != knopFncDecl)
        {
            // We only expect to find a function body block in a parameter scope block.
            Assert(pnodeChild->nop == knopBlock
                && (pnodeBlock->sxBlock.blockType == PnodeBlockType::Parameter
                    || pnodeChild->sxBlock.blockType == PnodeBlockType::Function));
            pnodeChild = pnodeChild->sxBlock.pnodeNext;
            continue;
        }
        AssertOrFailFast(i < nestedCount);

        if (pnodeChild->sxFnc.pnodeBody != nullptr)
        {
            // Anomalous case of a non-deferred function nested within a deferred one.
            // Work around by discarding the stub tree.
            return nullptr;
        }

        if (pnodeChild->sxFnc.IsGeneratedDefault())
        {
            ++i;
            pnodeChild = pnodeChild->sxFnc.pnodeNext;
            continue;
        }

        AnalysisAssertOrFailFast(i < nestedCount);

        deferredStubs[i].fncFlags = pnodeChild->sxFnc.fncFlags;
        deferredStubs[i].nestedCount = pnodeChild->sxFnc.nestedCount;
        deferredStubs[i].restorePoint = *pnodeChild->sxFnc.pRestorePoint;
        deferredStubs[i].deferredStubs = BuildDeferredStubTree(pnodeChild, recycler);
        deferredStubs[i].ichMin = pnodeChild->ichMin;
        ++i;
        pnodeChild = pnodeChild->sxFnc.pnodeNext;
    }

    return deferredStubs;
}