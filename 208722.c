bool Parser::FastScanFormalsAndBody()
{
    // The scanner is currently pointing just past the name of a function.
    // The idea here is to find the end of the function body as quickly as possible,
    // by tokenizing and tracking {}'s if possible.
    // String templates require some extra logic but can be handled.

    // The real wrinkle is "/" and "/=", which may indicate either a RegExp literal or a division, depending
    // on the context.
    // To handle this with minimal work, keep track of the last ";" seen at each {} depth. If we see one of the
    // difficult tokens, rewind to the last ";" at the current {} depth and parse statements until we pass the
    // point where we had to rewind. This will process the "/" as required.

    RestorePoint funcStart;
    m_pscan->Capture(&funcStart);

    const int maxRestorePointDepth = 16;
    struct FastScanRestorePoint
    {
        RestorePoint restorePoint;
        uint parenDepth;
        Js::LocalFunctionId functionId;
        int blockId;

        FastScanRestorePoint() : restorePoint(), parenDepth(0) {};
    };
    FastScanRestorePoint lastSColonAtCurlyDepth[maxRestorePointDepth];

    charcount_t ichStart = m_pscan->IchMinTok();
    uint blockIdSave = m_nextBlockId;
    uint functionIdSave = *m_nextFunctionId;
    uint curlyDepth = 0;
    uint strTmplDepth = 0;
    for (;;)
    {
        switch (m_token.tk)
        {
            case tkStrTmplBegin:
                UInt32Math::Inc(strTmplDepth, Parser::OutOfMemory);
                // Fall through

            case tkStrTmplMid:
            case tkLCurly:
                UInt32Math::Inc(curlyDepth, Parser::OutOfMemory);
                Int32Math::Inc(m_nextBlockId, &m_nextBlockId);
                break;

            case tkStrTmplEnd:
                // We can assert here, because the scanner will only return this token if we've told it we're
                // in a string template.
                Assert(strTmplDepth > 0);
                strTmplDepth--;
                break;

            case tkRCurly:
                if (curlyDepth == 1)
                {
                    Assert(strTmplDepth == 0);
                    if (PHASE_TRACE1(Js::ParallelParsePhase))
                    {
                        Output::Print(_u("Finished fast seek: %d. %s -- %d...%d\n"),
                                      m_currentNodeFunc->sxFnc.functionId,
                                      GetFunctionName(m_currentNodeFunc, m_currentNodeFunc->sxFnc.hint),
                                      ichStart, m_pscan->IchLimTok());
                    }
                    return true;
                }
                if (curlyDepth < maxRestorePointDepth)
                {
                    lastSColonAtCurlyDepth[curlyDepth].restorePoint.m_ichMinTok = (uint)-1;
                }
                curlyDepth--;
                if (strTmplDepth > 0)
                {
                    m_pscan->SetScanState(Scanner_t::ScanState::ScanStateStringTemplateMiddleOrEnd);
                }
                break;

            case tkSColon:
                // Track the location of the ";" (if it's outside parens, as we don't, for instance, want
                // to track the ";"'s in a for-loop header. If we find it's important to rewind within a paren
                // expression, we can do something more sophisticated.)
                if (curlyDepth < maxRestorePointDepth && lastSColonAtCurlyDepth[curlyDepth].parenDepth == 0)
                {
                    m_pscan->Capture(&lastSColonAtCurlyDepth[curlyDepth].restorePoint);
                    lastSColonAtCurlyDepth[curlyDepth].functionId = *this->m_nextFunctionId;
                    lastSColonAtCurlyDepth[curlyDepth].blockId = m_nextBlockId;
                }
                break;

            case tkLParen:
                if (curlyDepth < maxRestorePointDepth)
                {
                    UInt32Math::Inc(lastSColonAtCurlyDepth[curlyDepth].parenDepth);
                }
                break;

            case tkRParen:
                if (curlyDepth < maxRestorePointDepth)
                {
                    Assert(lastSColonAtCurlyDepth[curlyDepth].parenDepth != 0);
                    lastSColonAtCurlyDepth[curlyDepth].parenDepth--;
                }
                break;

            case tkID:
            {
                charcount_t tokLength = m_pscan->IchLimTok() - m_pscan->IchMinTok();
                // Detect the function and class keywords so we can track function ID's.
                // (In fast mode, the scanner doesn't distinguish keywords and doesn't point the token
                // to a PID.)
                // Detect try/catch/for to increment block count for them.
                switch (tokLength)
                {
                case 3:
                    if (!memcmp(m_pscan->PchMinTok(), "try", 3) || !memcmp(m_pscan->PchMinTok(), "for", 3))
                    {
                        Int32Math::Inc(m_nextBlockId, &m_nextBlockId);
                    }
                    break;
                case 5:
                    if (!memcmp(m_pscan->PchMinTok(), "catch", 5))
                    {
                        Int32Math::Inc(m_nextBlockId, &m_nextBlockId);
                    }
                    else if (!memcmp(m_pscan->PchMinTok(), "class", 5))
                    {
                        Int32Math::Inc(m_nextBlockId, &m_nextBlockId);
                        Int32Math::Inc(*this->m_nextFunctionId, (int*)this->m_nextFunctionId);
                    }
                    break;
                case 8:
                    if (!memcmp(m_pscan->PchMinTok(), "function", 8))
                    {
                        // Account for the possible func expr scope or dummy block for missing {}'s around a declaration
                        Int32Math::Inc(m_nextBlockId, &m_nextBlockId);
                        Int32Math::Inc(*this->m_nextFunctionId, (int*)this->m_nextFunctionId);
                    }
                    break;
                }
                break;
            }

            case tkDArrow:
                Int32Math::Inc(m_nextBlockId, &m_nextBlockId);
                Int32Math::Inc(*this->m_nextFunctionId, (int*)this->m_nextFunctionId);
                break;

            case tkDiv:
            case tkAsgDiv:
            {
                int opl;
                OpCode nop;
                tokens tkPrev = m_pscan->m_tkPrevious;
                if ((m_pscan->m_phtbl->TokIsBinop(tkPrev, &opl, &nop) && nop != knopNone) ||
                    (m_pscan->m_phtbl->TokIsUnop(tkPrev, &opl, &nop) &&
                     nop != knopNone &&
                     tkPrev != tkInc &&
                     tkPrev != tkDec) ||
                    tkPrev == tkColon ||
                    tkPrev == tkLParen ||
                    tkPrev == tkLBrack ||
                    tkPrev == tkRETURN)
                {
                    // Previous token indicates that we're starting an expression here and can't have a
                    // binary operator now.
                    // Assume this is a RegExp.
                    ParseRegExp<false>();
                    break;
                }
                uint tempCurlyDepth = curlyDepth < maxRestorePointDepth ? curlyDepth : maxRestorePointDepth - 1;
                for (; tempCurlyDepth != (uint)-1; tempCurlyDepth--)
                {
                    // We don't know whether we've got a RegExp or a divide. Rewind to the last safe ";"
                    // if we can and parse statements until we pass this point.
                    if (lastSColonAtCurlyDepth[tempCurlyDepth].restorePoint.m_ichMinTok != -1)
                    {
                        break;
                    }
                }
                if (tempCurlyDepth != (uint)-1)
                {
                    ParseNodePtr pnodeFncSave = m_currentNodeFunc;
                    int32 *pastSizeSave = m_pCurrentAstSize;
                    uint *pnestedCountSave = m_pnestedCount;
                    ParseNodePtr *ppnodeScopeSave = m_ppnodeScope;
                    ParseNodePtr *ppnodeExprScopeSave = m_ppnodeExprScope;

                    ParseNodePtr pnodeFnc = CreateDummyFuncNode(true);
                    m_ppnodeScope = &pnodeFnc->sxFnc.pnodeScopes;
                    m_ppnodeExprScope = nullptr;

                    charcount_t ichStop = m_pscan->IchLimTok();
                    curlyDepth = tempCurlyDepth;
                    m_pscan->SeekTo(lastSColonAtCurlyDepth[tempCurlyDepth].restorePoint);
                    m_nextBlockId = lastSColonAtCurlyDepth[tempCurlyDepth].blockId;
                    *this->m_nextFunctionId = lastSColonAtCurlyDepth[tempCurlyDepth].functionId;

                    ParseNodePtr pnodeBlock = StartParseBlock<true>(PnodeBlockType::Function, ScopeType_FunctionBody);

                    m_pscan->Scan();
                    do
                    {
                        ParseStatement<false>();
                    }
                    while(m_pscan->IchMinTok() < ichStop);

                    FinishParseBlock(pnodeBlock);

                    m_currentNodeFunc = pnodeFncSave;
                    m_pCurrentAstSize = pastSizeSave;
                    m_pnestedCount = pnestedCountSave;
                    m_ppnodeScope = ppnodeScopeSave;
                    m_ppnodeExprScope = ppnodeExprScopeSave;

                    // We've already consumed the first token of the next statement, so just continue
                    // without a further scan.
                    continue;
                }
            }

                // fall through to rewind to function start
            case tkScanError:
            case tkEOF:
                // Unexpected token.
                if (PHASE_TRACE1(Js::ParallelParsePhase))
                {
                    Output::Print(_u("Failed fast seek: %d. %s -- %d...%d\n"),
                                  m_currentNodeFunc->sxFnc.functionId,
                                  GetFunctionName(m_currentNodeFunc, m_currentNodeFunc->sxFnc.hint),
                                  ichStart, m_pscan->IchLimTok());
                }
                m_nextBlockId = blockIdSave;
                *m_nextFunctionId = functionIdSave;
                m_pscan->SeekTo(funcStart);
                return false;
        }

        m_pscan->ScanNoKeywords();
    }
}