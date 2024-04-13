void Parser::RestorePidRefForSym(Symbol *sym)
{
    IdentPtr pid = m_pscan->m_phtbl->PidHashNameLen(sym->GetName().GetBuffer(), sym->GetName().GetLength());
    Assert(pid);
    sym->SetPid(pid);
    PidRefStack *ref = this->PushPidRef(pid);
    ref->SetSym(sym);
}