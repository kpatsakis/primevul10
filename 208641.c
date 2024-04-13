void Parser::ThrowNewTargetSyntaxErrForGlobalScope()
{
    if (GetCurrentNonLambdaFunctionNode() != nullptr)
    {
        return;
    }

    if ((this->m_grfscr & fscrEval) != 0)
    {
        Js::JavascriptFunction * caller = nullptr;
        if (Js::JavascriptStackWalker::GetCaller(&caller, m_scriptContext))
        {
            Js::FunctionBody * callerBody = caller->GetFunctionBody();
            Assert(callerBody);
            if (!callerBody->GetIsGlobalFunc() && !(callerBody->IsLambda() && callerBody->GetEnclosedByGlobalFunc()))
            {
                return;
            }
        }
    }

    Error(ERRInvalidNewTarget);
 }