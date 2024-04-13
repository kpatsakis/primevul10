void Parser::FinishBackgroundPidRefs(BackgroundParseItem *item, bool isOtherParser)
{
    for (BlockInfoStack *blockInfo = item->GetParseContext()->currentBlockInfo; blockInfo; blockInfo = blockInfo->pBlockInfoOuter)
    {
        if (isOtherParser)
        {
            this->BindPidRefs<true>(blockInfo, item->GetMaxBlockId());
        }
        else
        {
            this->BindPidRefs<false>(blockInfo, item->GetMaxBlockId());
        }
    }
}