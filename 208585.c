void PnFnc::SetFuncSymbol(Symbol *sym)
{
    Assert(pnodeName &&
           pnodeName->nop == knopVarDecl);
    pnodeName->sxVar.sym = sym;
}