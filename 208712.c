ParseNodePtr Parser::ParsePostfixOperators(
    ParseNodePtr pnode,
    BOOL fAllowCall,
    BOOL fInNew,
    BOOL isAsyncExpr,
    BOOL *pfCanAssign,
    _Inout_ IdentToken* pToken,
    _Out_opt_ bool* pfIsDotOrIndex /*= nullptr */)
{
    uint16 count = 0;
    bool callOfConstants = false;
    if (pfIsDotOrIndex)
    {
        *pfIsDotOrIndex = false;
    }

    for (;;)
    {
        uint16 spreadArgCount = 0;
        switch (m_token.tk)
        {
        case tkLParen:
            {
                if (fInNew)
                {
                    ParseNodePtr pnodeArgs = ParseArgList<buildAST>(&callOfConstants, &spreadArgCount, &count);
                    if (buildAST)
                    {
                        Assert(pnode->nop == knopNew);
                        Assert(pnode->sxCall.pnodeArgs == nullptr);
                        pnode->sxCall.pnodeArgs = pnodeArgs;
                        pnode->sxCall.callOfConstants = callOfConstants;
                        pnode->sxCall.isApplyCall = false;
                        pnode->sxCall.isEvalCall = false;
                        pnode->sxCall.argCount = count;
                        pnode->sxCall.spreadArgCount = spreadArgCount;
                        pnode->ichLim = m_pscan->IchLimTok();
                    }
                    else
                    {
                        pnode = nullptr;
                        pToken->tk = tkNone; // This is no longer an identifier
                    }
                    fInNew = FALSE;
                    ChkCurTok(tkRParen, ERRnoRparen);
                }
                else
                {
                    bool fCallIsEval = false;
                    if (!fAllowCall)
                    {
                        return pnode;
                    }

                    uint saveNextBlockId = m_nextBlockId;
                    uint saveCurrBlockId = GetCurrentBlock()->sxBlock.blockId;

                    if (isAsyncExpr)
                    {
                        // Advance the block ID here in case this parenthetical expression turns out to be a lambda parameter list.
                        // That way the pid ref stacks will be created in their correct final form, and we can simply fix
                        // up function ID's.
                        GetCurrentBlock()->sxBlock.blockId = m_nextBlockId++;
                    }

                    ParseNodePtr pnodeArgs = ParseArgList<buildAST>(&callOfConstants, &spreadArgCount, &count);
                    // We used to un-defer a deferred function body here if it was called as part of the expression that declared it.
                    // We now detect this case up front in ParseFncDecl, which is cheaper and simpler.
                    if (buildAST)
                    {
                        pnode = CreateCallNode(knopCall, pnode, pnodeArgs);
                        Assert(pnode);

                        // Detect call to "eval" and record it on the function.
                        // Note: we used to leave it up to the byte code generator to detect eval calls
                        // at global scope, but now it relies on the flag the parser sets, so set it here.

                        if (count > 0 && this->NodeIsEvalName(pnode->sxCall.pnodeTarget))
                        {
                            this->MarkEvalCaller();
                            fCallIsEval = true;
                        }

                        pnode->sxCall.callOfConstants = callOfConstants;
                        pnode->sxCall.spreadArgCount = spreadArgCount;
                        pnode->sxCall.isApplyCall = false;
                        pnode->sxCall.isEvalCall = fCallIsEval;
                        pnode->sxCall.argCount = count;
                        pnode->ichLim = m_pscan->IchLimTok();
                    }
                    else
                    {
                        pnode = nullptr;
                        if (pToken->tk == tkID && pToken->pid == wellKnownPropertyPids.eval && count > 0) // Detect eval
                        {
                            this->MarkEvalCaller();
                        }
                        pToken->tk = tkNone; // This is no longer an identifier
                    }

                    ChkCurTok(tkRParen, ERRnoRparen);

                    if (isAsyncExpr)
                    {
                        GetCurrentBlock()->sxBlock.blockId = saveCurrBlockId;
                        if (m_token.tk == tkDArrow)
                        {
                            // We're going to rewind and reinterpret the expression as a parameter list.
                            // Put back the original next-block-ID so the existing pid ref stacks will be correct.
                            m_nextBlockId = saveNextBlockId;
                        }
                    }
                }
                if (pfCanAssign)
                {
                    *pfCanAssign = FALSE;
                }
                if (pfIsDotOrIndex)
                {
                    *pfIsDotOrIndex = false;
                }
                break;
            }
        case tkLBrack:
            {
                m_pscan->Scan();
                ParseNodePtr pnodeExpr = ParseExpr<buildAST>();
                if (buildAST)
                {
                    pnode = CreateBinNode(knopIndex, pnode, pnodeExpr);
                    pnode->ichLim = m_pscan->IchLimTok();
                }
                else
                {
                    pnode = nullptr;
                    pToken->tk = tkNone; // This is no longer an identifier
                }
                ChkCurTok(tkRBrack, ERRnoRbrack);
                if (pfCanAssign)
                {
                    *pfCanAssign = TRUE;
                }
                if (pfIsDotOrIndex)
                {
                    *pfIsDotOrIndex = true;
                }

                if (!buildAST)
                {
                    break;
                }

                bool shouldConvertToDot = false;
                if (pnode->sxBin.pnode2->nop == knopStr)
                {
                    // if the string is empty or contains escape character, we will not convert them to dot node
                    shouldConvertToDot = pnode->sxBin.pnode2->sxPid.pid->Cch() > 0 && !m_pscan->IsEscapeOnLastTkStrCon();
                }

                if (shouldConvertToDot)
                {
                    LPCOLESTR str = pnode->sxBin.pnode2->sxPid.pid->Psz();
                    // See if we can convert o["p"] into o.p and o["0"] into o[0] since they're equivalent and the latter forms
                    // are faster
                    uint32 uintValue;
                    if(Js::JavascriptOperators::TryConvertToUInt32(
                           str,
                           pnode->sxBin.pnode2->sxPid.pid->Cch(),
                           &uintValue) &&
                       !Js::TaggedInt::IsOverflow(uintValue)) // the optimization is not very useful if the number can't be represented as a TaggedInt
                    {
                        // No need to verify that uintValue != JavascriptArray::InvalidIndex since all nonnegative TaggedInts are valid indexes
                        auto intNode = CreateIntNodeWithScanner(uintValue); // implicit conversion from uint32 to int32
                        pnode->sxBin.pnode2 = intNode;
                    }
                    // Field optimization (see GlobOpt::KillLiveElems) checks for value being a Number,
                    // and since NaN/Infinity is a number it won't kill o.NaN/o.Infinity which would cause a problem
                    // if we decide to hoist o.NaN/o.Infinity.
                    // We need to keep o["NaN"] and o["+/-Infinity"] as array element access (we don't hoist that but we may hoist field access),
                    // so no matter if it's killed by o[x] inside a loop, we make sure that we never hoist these.
                    // We need to follow same logic for strings that convert to a floating point number.
                    else
                    {
                        bool doConvertToProperty = false;    // Convert a["x"] -> a.x.
                        if (!Parser::IsNaNOrInfinityLiteral<true>(str))
                        {
                            const OLECHAR* terminalChar;
                            double dbl = Js::NumberUtilities::StrToDbl(str, &terminalChar, m_scriptContext);
                            bool convertsToFloat = !Js::NumberUtilities::IsNan(dbl);
                            doConvertToProperty = !convertsToFloat;
                        }

                        if (doConvertToProperty)
                        {
                            pnode->sxBin.pnode2->nop = knopName;
                            pnode->nop = knopDot;
                            pnode->grfpn |= PNodeFlags::fpnIndexOperator;
                        }
                    }
                }
            }
            break;

        case tkDot:
            {
            ParseNodePtr name = nullptr;
            OpCode opCode = knopDot;

            m_pscan->Scan();
            if (!m_token.IsIdentifier())
            {
                //allow reserved words in ES5 mode
                if (!(m_token.IsReservedWord()))
                {
                    IdentifierExpectedError(m_token);
                }
            }
            // Note: see comment above about field optimization WRT NaN/Infinity/-Infinity.
            // Convert a.Nan, a.Infinity into a["NaN"], a["Infinity"].
            // We don't care about -Infinity case here because x.-Infinity is invalid in JavaScript.
            // Both NaN and Infinity are identifiers.
            else if (buildAST && Parser::IsNaNOrInfinityLiteral<false>(m_token.GetIdentifier(m_phtbl)->Psz()))
            {
                opCode = knopIndex;
            }

            if (buildAST)
            {
                if (opCode == knopDot)
                {
                    name = CreateNameNode(m_token.GetIdentifier(m_phtbl));
                }
                else
                {
                    Assert(opCode == knopIndex);
                    name = CreateStrNodeWithScanner(m_token.GetIdentifier(m_phtbl));
                }
                pnode = CreateBinNode(opCode, pnode, name);
            }
            else
            {
                pnode = nullptr;
                pToken->tk = tkNone;
            }

            if (pfCanAssign)
            {
                *pfCanAssign = TRUE;
            }
            if (pfIsDotOrIndex)
            {
                *pfIsDotOrIndex = true;
            }
            m_pscan->Scan();

            break;
            }

        case tkStrTmplBasic:
        case tkStrTmplBegin:
            {
                ParseNode* templateNode = ParseStringTemplateDecl<buildAST>(pnode);

                if (!buildAST)
                {
                    pToken->tk = tkNone; // This is no longer an identifier
                }

                pnode = templateNode;
                if (pfCanAssign)
                {
                    *pfCanAssign = FALSE;
                }
                if (pfIsDotOrIndex)
                {
                    *pfIsDotOrIndex = false;
                }
                break;
            }
        default:
            return pnode;
        }
    }
}