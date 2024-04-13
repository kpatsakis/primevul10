void Parser::PushScope(Scope *scope)
{
    Assert(scope);
    scope->SetEnclosingScope(m_currentScope);
    m_currentScope = scope;
}