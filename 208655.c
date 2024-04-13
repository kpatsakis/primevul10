void Parser::PopScope(Scope *scope)
{
    Assert(scope == m_currentScope);
    m_currentScope = scope->GetEnclosingScope();
    scope->SetEnclosingScope(nullptr);
}