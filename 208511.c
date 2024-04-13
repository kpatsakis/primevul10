void Parser::CheckStrictFormalParameters()
{
    if (m_token.tk == tkID)
    {
        // single parameter arrow function case
        IdentPtr pid = m_token.GetIdentifier(m_phtbl);
        CheckStrictModeEvalArgumentsUsage(pid);
        return;
    }

    Assert(m_token.tk == tkLParen);
    m_pscan->ScanForcingPid();

    if (m_token.tk != tkRParen)
    {
        SList<IdentPtr> formals(&m_nodeAllocator);
        for (;;)
        {
            if (m_token.tk != tkID)
            {
                IdentifierExpectedError(m_token);
            }

            IdentPtr pid = m_token.GetIdentifier(m_phtbl);
            CheckStrictModeEvalArgumentsUsage(pid);
            if (formals.Has(pid))
            {
                Error(ERRES5ArgSame, m_pscan->IchMinTok(), m_pscan->IchLimTok());
            }
            else
            {
                formals.Prepend(pid);
            }

            m_pscan->Scan();

            if (m_token.tk == tkAsg && m_scriptContext->GetConfig()->IsES6DefaultArgsEnabled())
            {
                m_pscan->Scan();
                // We can avoid building the AST since we are just checking the default expression.
                ParseNodePtr pnodeInit = ParseExpr<false>(koplCma);
                Assert(pnodeInit == nullptr);
            }

            if (m_token.tk != tkComma)
            {
                break;
            }
            m_pscan->ScanForcingPid();

            if (m_token.tk == tkRParen && m_scriptContext->GetConfig()->IsES7TrailingCommaEnabled())
            {
                break;
            }
        }
    }
    Assert(m_token.tk == tkRParen);
}