void Parser::InitNode(OpCode nop,ParseNodePtr pnode) {
    pnode->nop = nop;
    pnode->grfpn = PNodeFlags::fpnNone;
    pnode->location = NoRegister;
    pnode->emitLabels = false;
    pnode->isUsed = true;
    pnode->notEscapedUse = false;
    pnode->isInList = false;
    pnode->isCallApplyTargetLoad = false;
}