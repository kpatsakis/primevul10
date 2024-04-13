Symbol *PnFnc::GetFuncSymbol()
{
    if (pnodeName &&
        pnodeName->nop == knopVarDecl)
    {
        return pnodeName->sxVar.sym;
    }
    return nullptr;
}