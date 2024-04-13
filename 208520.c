ParseNodePtr PnFnc::GetBodyScope() const
{
    if (this->pnodeBodyScope == nullptr)
    {
        return nullptr;
    }
    Assert(this->pnodeBodyScope->nop == knopBlock &&
           this->pnodeBodyScope->sxBlock.pnodeNext == nullptr);
    return this->pnodeBodyScope->sxBlock.pnodeScopes;
}