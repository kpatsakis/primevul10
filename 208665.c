ParseNodePtr PnFnc::GetParamScope() const
{
    if (this->pnodeScopes == nullptr)
    {
        return nullptr;
    }
    Assert(this->pnodeScopes->nop == knopBlock &&
           this->pnodeScopes->sxBlock.pnodeNext == nullptr);
    return this->pnodeScopes->sxBlock.pnodeScopes;
}