void ParseNode::Dump()
{
    switch(nop)
    {
    case knopFncDecl:
    case knopProg:
        LPCOLESTR name = Js::Constants::AnonymousFunction;
        if(this->sxFnc.pnodeName)
        {
            name = this->sxFnc.pnodeName->sxVar.pid->Psz();
        }

        Output::Print(_u("%s (%d) [%d, %d]:\n"), name, this->sxFnc.functionId, this->sxFnc.lineNumber, this->sxFnc.columnNumber);
        Output::Print(_u("hasArguments: %s callsEval:%s childCallsEval:%s HasReferenceableBuiltInArguments:%s ArgumentsObjectEscapes:%s HasWith:%s HasThis:%s HasOnlyThis:%s \n"),
            IsTrueOrFalse(this->sxFnc.HasHeapArguments()),
            IsTrueOrFalse(this->sxFnc.CallsEval()),
            IsTrueOrFalse(this->sxFnc.ChildCallsEval()),
            IsTrueOrFalse(this->sxFnc.HasReferenceableBuiltInArguments()),
            IsTrueOrFalse(this->sxFnc.GetArgumentsObjectEscapes()),
            IsTrueOrFalse(this->sxFnc.HasWithStmt()),
            IsTrueOrFalse(this->sxFnc.HasThisStmt()),
            IsTrueOrFalse(this->sxFnc.HasOnlyThisStmts()));
        if(this->sxFnc.funcInfo)
        {
            this->sxFnc.funcInfo->Dump();
        }
        break;
    }
}