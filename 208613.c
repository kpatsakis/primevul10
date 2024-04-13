void Parser::EnsureStackAvailable()
{
    if (!m_scriptContext->GetThreadContext()->IsStackAvailable(Js::Constants::MinStackCompile))
    {
        Error(ERRnoMemory);
    }
}