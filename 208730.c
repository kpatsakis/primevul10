ParseNodePtr Parser::CreateNameNode(IdentPtr pid,charcount_t ichMin,charcount_t ichLim) {
  ParseNodePtr pnode = CreateNodeT<knopName>(ichMin,ichLim);
  pnode->sxPid.pid = pid;
  pnode->sxPid.sym=NULL;
  pnode->sxPid.symRef=NULL;
  return pnode;
}