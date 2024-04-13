void PnPid::SetSymRef(PidRefStack *ref)
{
    Assert(symRef == nullptr);
    this->symRef = ref->GetSymRef();
}