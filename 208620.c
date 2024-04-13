Js::PropertyId PnPid::PropertyIdFromNameNode() const
{
    Js::PropertyId propertyId;
    Symbol *sym = this->sym;
    if (sym)
    {
        propertyId = sym->GetPosition();
    }
    else
    {
        propertyId = this->pid->GetPropertyId();
    }
    return propertyId;
}