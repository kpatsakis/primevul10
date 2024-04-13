ParseNodePtr Parser::ParseArrayList(bool *pArrayOfTaggedInts, bool *pArrayOfInts, bool *pArrayOfNumbers, bool *pHasMissingValues, uint *count, uint *spreadCount)
{
    ParseNodePtr pnodeArg = nullptr;
    ParseNodePtr pnodeList = nullptr;
    ParseNodePtr *lastNodeRef = nullptr;

    *count = 0;

    // Check for an empty list
    if (tkRBrack == m_token.tk)
    {
        return nullptr;
    }

    this->m_arrayDepth++;
    bool arrayOfTaggedInts = buildAST;
    bool arrayOfInts = buildAST;
    bool arrayOfNumbers = buildAST;
    bool arrayOfVarInts = false;
    bool hasMissingValues = false;

    for (;;)
    {
        (*count)++;
        if (tkComma == m_token.tk || tkRBrack == m_token.tk)
        {
            hasMissingValues = true;
            arrayOfTaggedInts = false;
            arrayOfInts = false;
            arrayOfNumbers = false;
            if (buildAST)
            {
                pnodeArg = CreateNodeWithScanner<knopEmpty>();
            }
        }
        else
        {
            // Allow Spread in array literals.
            pnodeArg = ParseExpr<buildAST>(koplCma, nullptr, TRUE, /* fAllowEllipsis */ TRUE);
            if (buildAST)
            {
                if (pnodeArg->nop == knopEllipsis)
                {
                    (*spreadCount)++;
                }
                this->CheckArguments(pnodeArg);
            }
        }

#if DEBUG
        if(m_grfscr & fscrEnforceJSON && !IsJSONValid(pnodeArg))
        {
            Error(ERRsyntax);
        }
#endif

        if (buildAST)
        {
            if (arrayOfNumbers)
            {
                if (pnodeArg->nop != knopInt)
                {
                    arrayOfTaggedInts = false;
                    if (pnodeArg->nop != knopFlt)
                    {
                        // Not an array of constants.
                        arrayOfInts = false;
                        arrayOfNumbers = false;
                    }
                    else if (arrayOfInts && Js::JavascriptNumber::IsInt32OrUInt32(pnodeArg->sxFlt.dbl) && (!Js::JavascriptNumber::IsInt32(pnodeArg->sxFlt.dbl) || pnodeArg->sxFlt.dbl == -2147483648.0))
                    {
                        // We've seen nothing but ints, and this is a uint32 but not an int32.
                        // Unless we see an actual float at some point, we want an array of vars
                        // so we can work with tagged ints.
                        arrayOfVarInts = true;
                    }
                    else
                    {
                        // Not an int array, but it may still be a float array.
                        arrayOfInts = false;
                    }
                }
                else
                {
                    if (Js::SparseArraySegment<int32>::IsMissingItem((int32*)&pnodeArg->sxInt.lw))
                    {
                        arrayOfInts = false;
                    }
                    if (Js::TaggedInt::IsOverflow(pnodeArg->sxInt.lw))
                    {
                        arrayOfTaggedInts = false;
                    }
                }
            }
            AddToNodeListEscapedUse(&pnodeList, &lastNodeRef, pnodeArg);
        }

        if (tkComma != m_token.tk)
        {
            break;
        }
        m_pscan->Scan();

        if (tkRBrack == m_token.tk)
        {
            break;
        }
    }

    if (spreadCount != nullptr && *spreadCount > 0){
        CHAKRATEL_LANGSTATS_INC_LANGFEATURECOUNT(SpreadFeature, m_scriptContext);
    }

    if (buildAST)
    {
        AssertMem(lastNodeRef);
        AssertNodeMem(*lastNodeRef);
        pnodeList->ichLim = (*lastNodeRef)->ichLim;

        if (arrayOfVarInts && arrayOfInts)
        {
            arrayOfInts = false;
            arrayOfNumbers = false;
        }
        *pArrayOfTaggedInts = arrayOfTaggedInts;
        *pArrayOfInts = arrayOfInts;
        *pArrayOfNumbers = arrayOfNumbers;
        *pHasMissingValues = hasMissingValues;
    }
    this->m_arrayDepth--;
    return pnodeList;
}