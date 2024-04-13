void Parser::SetPidRefsInScopeDynamic(IdentPtr pid, int blockId)
{
    PidRefStack *ref = pid->GetTopRef();
    while (ref && ref->GetScopeId() >= blockId)
    {
        ref->SetDynamicBinding();
        ref = ref->prev;
    }
}