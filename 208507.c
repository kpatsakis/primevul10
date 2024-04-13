void Parser::TrackAssignment(ParseNodePtr pnodeT, IdentToken* pToken)
{
    if (buildAST)
    {
        Assert(pnodeT != nullptr);
        if (pnodeT->nop == knopName)
        {
            PidRefStack *ref = pnodeT->sxPid.pid->GetTopRef();
            Assert(ref);
            ref->isAsg = true;
        }
    }
    else
    {
        Assert(pToken != nullptr);
        if (pToken->tk == tkID)
        {
            PidRefStack *ref = pToken->pid->GetTopRef();
            Assert(ref);
            ref->isAsg = true;
        }
    }
}