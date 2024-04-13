ParseNodePtr Parser::ParseDestructuredVarDecl(tokens declarationType, bool isDecl, bool *hasSeenRest, bool topLevel/* = true*/, bool allowEmptyExpression/* = true*/)
{
    ParseNodePtr pnodeElem = nullptr;
    int parenCount = 0;
    bool seenRest = false;

    // Save the Block ID prior to the increments, so we can restore it back.
    int originalCurrentBlockId = GetCurrentBlock()->sxBlock.blockId;

    // Eat the left parentheses only when its not a declaration. This will make sure we throw syntax errors early.
    if (!isDecl)
    {
        while (m_token.tk == tkLParen)
        {
            m_pscan->Scan();
            ++parenCount;

            // Match the block increment we do upon entering parenthetical expressions
            // so that the block ID's will match on reparsing of parameters.
            GetCurrentBlock()->sxBlock.blockId = m_nextBlockId++;
        }
    }

    if (m_token.tk == tkEllipsis)
    {
        // As per ES 2015 : Rest can have left-hand-side-expression when on assignment expression, but under declaration only binding identifier is allowed
        // But spec is going to change for this one to allow LHS-expression both on expression and declaration - so making that happen early.

        seenRest = true;
        m_pscan->Scan();

        // Eat the left parentheses only when its not a declaration. This will make sure we throw syntax errors early.
        if (!isDecl)
        {
            while (m_token.tk == tkLParen)
            {
                m_pscan->Scan();
                ++parenCount;

                // Match the block increment we do upon entering parenthetical expressions
                // so that the block ID's will match on reparsing of parameters.
                GetCurrentBlock()->sxBlock.blockId = m_nextBlockId++;
            }
        }

        if (m_token.tk != tkID && m_token.tk != tkSUPER && m_token.tk != tkLCurly && m_token.tk != tkLBrack)
        {
            if (isDecl)
            {
                Error(ERRnoIdent);
            }
            else
            {
                Error(ERRInvalidAssignmentTarget);
            }
        }
    }

    if (IsPossiblePatternStart())
    {
        // Go recursively
        pnodeElem = ParseDestructuredLiteral<buildAST>(declarationType, isDecl, false /*topLevel*/, seenRest ? DIC_ShouldNotParseInitializer : DIC_None);
        if (!isDecl)
        {
            BOOL fCanAssign;
            IdentToken token;
            // Look for postfix operator
            pnodeElem = ParsePostfixOperators<buildAST>(pnodeElem, TRUE, FALSE, FALSE, &fCanAssign, &token);
        }
    }
    else if (m_token.tk == tkSUPER || m_token.tk == tkID)
    {
        if (isDecl)
        {
            charcount_t ichMin = m_pscan->IchMinTok();
            pnodeElem = ParseVariableDeclaration<buildAST>(declarationType, ichMin
                ,/* fAllowIn */false, /* pfForInOk */nullptr, /* singleDefOnly */true, /* allowInit */!seenRest, false /*topLevelParse*/);

        }
        else
        {
            BOOL fCanAssign;
            IdentToken token;
            // We aren't declaring anything, so scan the ID reference manually.
            pnodeElem = ParseTerm<buildAST>(/* fAllowCall */ m_token.tk != tkSUPER, nullptr /*pNameHint*/, nullptr /*pHintLength*/, nullptr /*pShortNameOffset*/, &token, false,
                                                             &fCanAssign);

            // In this destructuring case we can force error here as we cannot assign.

            if (!fCanAssign)
            {
                Error(ERRInvalidAssignmentTarget);
            }

            if (buildAST)
            {
                if (IsStrictMode() && pnodeElem != nullptr && pnodeElem->nop == knopName)
                {
                    CheckStrictModeEvalArgumentsUsage(pnodeElem->sxPid.pid);
                }
            }
            else
            {
                if (IsStrictMode() && token.tk == tkID)
                {
                    CheckStrictModeEvalArgumentsUsage(token.pid);
                }
                token.tk = tkNone;
            }
        }
    }
    else if (!((m_token.tk == tkComma || m_token.tk == tkRBrack || m_token.tk == tkRCurly) && allowEmptyExpression))
    {
        if (m_token.IsOperator())
        {
            Error(ERRDestructNoOper);
        }
        Error(ERRDestructIDRef);
    }

    // Swallow RParens before a default expression, if any.
    // We eat the left parentheses only when its not a declaration. This will make sure we throw syntax errors early. We need to do the same for right parentheses.
    if (!isDecl)
    {
        while (m_token.tk == tkRParen)
        {
            m_pscan->Scan();
            --parenCount;
        }
    }

    if (hasSeenRest != nullptr)
    {
        *hasSeenRest = seenRest;
    }

    if (m_token.tk == tkAsg)
    {
        // Parse the initializer.
        if (seenRest)
        {
            Error(ERRRestWithDefault);
        }
        m_pscan->Scan();

        bool alreadyHasInitError = m_hasDeferredShorthandInitError;
        ParseNodePtr pnodeInit = ParseExpr<buildAST>(koplCma);

        if (m_hasDeferredShorthandInitError && !alreadyHasInitError)
        {
            Error(ERRnoColon);
        }

        if (buildAST)
        {
            pnodeElem = CreateBinNode(knopAsg, pnodeElem, pnodeInit);
        }
    }

    if (buildAST && seenRest)
    {
        ParseNodePtr pnodeRest = CreateNodeWithScanner<knopEllipsis>();
        pnodeRest->sxUni.pnode1 = pnodeElem;
        pnodeElem = pnodeRest;
    }

    // We eat the left parentheses only when its not a declaration. This will make sure we throw syntax errors early. We need to do the same for right parentheses.
    if (!isDecl)
    {
        while (m_token.tk == tkRParen)
        {
            m_pscan->Scan();
            --parenCount;
        }

        // Restore the Block ID of the current block after the parsing of destructured variable declarations and initializers.
        GetCurrentBlock()->sxBlock.blockId = originalCurrentBlockId;
    }

    if (!(m_token.tk == tkComma || m_token.tk == tkRBrack || m_token.tk == tkRCurly))
    {
        if (m_token.IsOperator())
        {
            Error(ERRDestructNoOper);
        }
        Error(ERRsyntax);
    }

    if (parenCount != 0)
    {
        Error(ERRnoRparen);
    }
    return pnodeElem;
}