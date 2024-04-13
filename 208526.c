bool Parser::IsES6DestructuringEnabled() const
{
    return m_scriptContext->GetConfig()->IsES6DestructuringEnabled();
}