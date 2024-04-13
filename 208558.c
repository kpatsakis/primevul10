ParseNodePtr Parser::ParseArgList( bool *pCallOfConstants, uint16 *pSpreadArgCount, uint16 * pCount)
{
    ParseNodePtr pnodeArg;
    ParseNodePtr pnodeList = nullptr;
    ParseNodePtr *lastNodeRef = nullptr;

    // Check for an empty list
    Assert(m_token.tk == tkLParen);

    if (m_pscan->Scan() == tkRParen)
    {
        return nullptr;
    }

    *pCallOfConstants = true;
    *pSpreadArgCount = 0;

    int count=0;
    while (true)
    {
        // the count of arguments has to fit in an unsigned short
        if (count > 0xffffU)
            Error(ERRnoMemory);
        // Allow spread in argument lists.
        IdentToken token;
        pnodeArg = ParseExpr<buildAST>(koplCma, nullptr, TRUE, /* fAllowEllipsis */TRUE, NULL, nullptr, nullptr, &token);
        ++count;
        this->MarkEscapingRef(pnodeArg, &token);

        if (buildAST)
        {
            this->CheckArguments(pnodeArg);

            if (*pCallOfConstants && !IsConstantInFunctionCall(pnodeArg))
            {
                *pCallOfConstants = false;
            }

            if (pnodeArg->nop == knopEllipsis)
            {
                (*pSpreadArgCount)++;
            }

            AddToNodeListEscapedUse(&pnodeList, &lastNodeRef, pnodeArg);
        }
        if (m_token.tk != tkComma)
        {
            break;
        }
        m_pscan->Scan();

        if (m_token.tk == tkRParen && m_scriptContext->GetConfig()->IsES7TrailingCommaEnabled())
        {
            break;
        }
    }

    if (pSpreadArgCount!=nullptr && (*pSpreadArgCount) > 0){
        CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(SpreadFeature, m_scriptContext);
    }

    *pCount = static_cast<uint16>(count);
    if (buildAST)
    {
        AssertMem(lastNodeRef);
        AssertNodeMem(*lastNodeRef);
        pnodeList->ichLim = (*lastNodeRef)->ichLim;
    }

    return pnodeList;
}