bool Parser::ParseOptionalExpr(ParseNodePtr* pnode, bool fUnaryOrParen, int oplMin, BOOL *pfCanAssign, BOOL fAllowIn, BOOL fAllowEllipsis, _Inout_opt_ IdentToken* pToken)
{
    *pnode = nullptr;
    if (m_token.tk == tkRCurly ||
        m_token.tk == tkRBrack ||
        m_token.tk == tkRParen ||
        m_token.tk == tkSColon ||
        m_token.tk == tkColon ||
        m_token.tk == tkComma ||
        m_token.tk == tkLimKwd ||
        m_pscan->FHadNewLine())
    {
        return false;
    }

    IdentToken token;
    ParseNodePtr pnodeT = ParseExpr<buildAST>(oplMin, pfCanAssign, fAllowIn, fAllowEllipsis, nullptr /*pNameHint*/, nullptr /*pHintLength*/, nullptr /*pShortNameOffset*/, &token, fUnaryOrParen);
    // Detect nested function escapes of the pattern "return function(){...}" or "yield function(){...}".
    // Doing so in the parser allows us to disable stack-nested-functions in common cases where an escape
    // is not detected at byte code gen time because of deferred parsing.
    this->MarkEscapingRef(pnodeT, &token);
    if (pToken)
    {
        *pToken = token;
    }
    *pnode = pnodeT;
    return true;
}