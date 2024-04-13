ParseNodePtr Parser::ParseDestructuredArrayLiteral(tokens declarationType, bool isDecl, bool topLevel)
{
    Assert(m_token.tk == tkLBrack);
    charcount_t ichMin = m_pscan->IchMinTok();

    m_pscan->Scan();

    ParseNodePtr pnodeDestructArr = nullptr;
    ParseNodePtr pnodeList = nullptr;
    ParseNodePtr *lastNodeRef = nullptr;
    uint count = 0;
    bool hasMissingValues = false;
    bool seenRest = false;

    if (m_token.tk != tkRBrack)
    {
        while (true)
        {
            ParseNodePtr pnodeElem = ParseDestructuredVarDecl<buildAST>(declarationType, isDecl, &seenRest, topLevel);
            if (buildAST)
            {
                if (pnodeElem == nullptr && buildAST)
                {
                    pnodeElem = CreateNodeWithScanner<knopEmpty>();
                    hasMissingValues = true;
                }
                AddToNodeListEscapedUse(&pnodeList, &lastNodeRef, pnodeElem);
            }
            count++;

            if (m_token.tk == tkRBrack)
            {
                break;
            }

            if (m_token.tk != tkComma)
            {
                Error(ERRDestructNoOper);
            }

            if (seenRest) // Rest must be in the last position.
            {
                Error(ERRDestructRestLast);
            }

            m_pscan->Scan();

            // break if we have the trailing comma as well, eg. [a,]
            if (m_token.tk == tkRBrack)
            {
                break;
            }
        }
    }

    if (buildAST)
    {
        pnodeDestructArr = CreateNodeWithScanner<knopArrayPattern>();
        pnodeDestructArr->sxArrLit.pnode1 = pnodeList;
        pnodeDestructArr->sxArrLit.arrayOfTaggedInts = false;
        pnodeDestructArr->sxArrLit.arrayOfInts = false;
        pnodeDestructArr->sxArrLit.arrayOfNumbers = false;
        pnodeDestructArr->sxArrLit.hasMissingValues = hasMissingValues;
        pnodeDestructArr->sxArrLit.count = count;
        pnodeDestructArr->sxArrLit.spreadCount = seenRest ? 1 : 0;
        pnodeDestructArr->ichMin = ichMin;
        pnodeDestructArr->ichLim = m_pscan->IchLimTok();

        if (pnodeDestructArr->sxArrLit.pnode1)
        {
            this->CheckArguments(pnodeDestructArr->sxArrLit.pnode1);
        }
    }

    return pnodeDestructArr;
}