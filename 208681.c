void Parser::ParseExpressionLambdaBody(ParseNodePtr pnodeLambda)
{
    ParseNodePtr *lastNodeRef = nullptr;

    // The lambda body is a single expression, the result of which is the return value.
    ParseNodePtr pnodeRet = nullptr;

    if (buildAST)
    {
        pnodeRet = CreateNodeWithScanner<knopReturn>();
        pnodeRet->grfpn |= PNodeFlags::fpnSyntheticNode;
        pnodeLambda->sxFnc.pnodeScopes->sxBlock.pnodeStmt = pnodeRet;
    }

    IdentToken token;
    charcount_t lastRParen = 0;
    ParseNodePtr result = ParseExpr<buildAST>(koplAsg, nullptr, TRUE, FALSE, nullptr, nullptr, nullptr, &token, false, nullptr, &lastRParen);

    this->MarkEscapingRef(result, &token);

    if (buildAST)
    {
        pnodeRet->sxReturn.pnodeExpr = result;

        pnodeRet->ichMin = pnodeRet->sxReturn.pnodeExpr->ichMin;
        pnodeRet->ichLim = pnodeRet->sxReturn.pnodeExpr->ichLim;

        // Pushing a statement node with PushStmt<>() normally does this initialization
        // but do it here manually since we know there is no outer statement node.
        pnodeRet->sxStmt.grfnop = 0;
        pnodeRet->sxStmt.pnodeOuter = nullptr;

        pnodeLambda->ichLim = max(pnodeRet->ichLim, lastRParen);
        pnodeLambda->sxFnc.cbLim = m_pscan->IecpLimTokPrevious();
        pnodeLambda->sxFnc.pnodeScopes->ichLim = pnodeRet->ichLim;

        pnodeLambda->sxFnc.pnodeBody = nullptr;
        AddToNodeList(&pnodeLambda->sxFnc.pnodeBody, &lastNodeRef, pnodeLambda->sxFnc.pnodeScopes);

        // Append an EndCode node.
        ParseNodePtr end = CreateNodeWithScanner<knopEndCode>(pnodeRet->ichLim);
        end->ichLim = end->ichMin; // make end code zero width at the immediate end of lambda body
        AddToNodeList(&pnodeLambda->sxFnc.pnodeBody, &lastNodeRef, end);

        // Lambda's do not have arguments binding
        pnodeLambda->sxFnc.SetHasReferenceableBuiltInArguments(false);
    }
}