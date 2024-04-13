ParseNode *Parser::GetCurrentNonLambdaFunctionNode()
{
    if (m_currentNodeNonLambdaDeferredFunc != nullptr)
    {
        return m_currentNodeNonLambdaDeferredFunc;
    }
    return m_currentNodeNonLambdaFunc;

}