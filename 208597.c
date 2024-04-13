ParseNode *Parser::GetCurrentFunctionNode()
{
    if (m_currentNodeDeferredFunc != nullptr)
    {
        return m_currentNodeDeferredFunc;
    }
    else if (m_currentNodeFunc != nullptr)
    {
        return m_currentNodeFunc;
    }
    else
    {
        AssertMsg(GetFunctionBlock()->sxBlock.blockType == PnodeBlockType::Global,
            "Most likely we are trying to find a syntax error, related to 'let' or 'const' in deferred parsing mode with disabled support of 'let' and 'const'");
        return m_currentNodeProg;
    }
}