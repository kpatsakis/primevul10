ParseNode* Parser::CopyPnode(ParseNode *pnode) {
    if (pnode==NULL)
        return NULL;
    switch (pnode->nop) {
        //PTNODE(knopName       , "name"        ,None    ,Pid  ,fnopLeaf)
    case knopName: {
      ParseNode* nameNode=CreateNameNode(pnode->sxPid.pid,pnode->ichMin,pnode->ichLim);
      nameNode->sxPid.sym=pnode->sxPid.sym;
      return nameNode;
    }
      //PTNODE(knopInt        , "int const"    ,None    ,Int  ,fnopLeaf|fnopConst)
  case knopInt:
    return pnode;
      //PTNODE(knopFlt        , "flt const"    ,None    ,Flt  ,fnopLeaf|fnopConst)
  case knopFlt:
    return pnode;
      //PTNODE(knopStr        , "str const"    ,None    ,Pid  ,fnopLeaf|fnopConst)
  case knopStr:
    return pnode;
      //PTNODE(knopRegExp     , "reg expr"    ,None    ,Pid  ,fnopLeaf|fnopConst)
  case knopRegExp:
    return pnode;
    break;
      //PTNODE(knopThis       , "this"        ,None    ,None ,fnopLeaf)
  case knopThis:
    return CreateNodeT<knopThis>(pnode->ichMin,pnode->ichLim);
      //PTNODE(knopNull       , "null"        ,Null    ,None ,fnopLeaf)
  case knopNull:
    return pnode;
      //PTNODE(knopFalse      , "false"        ,False   ,None ,fnopLeaf)
  case knopFalse:
    {
      ParseNode* ret = CreateNodeT<knopFalse>(pnode->ichMin, pnode->ichLim);
      ret->location = pnode->location;
      return ret;
    }
      //PTNODE(knopTrue       , "true"        ,True    ,None ,fnopLeaf)
  case knopTrue:
    {
        ParseNode* ret = CreateNodeT<knopTrue>(pnode->ichMin, pnode->ichLim);
        ret->location = pnode->location;
        return ret;
    }
      //PTNODE(knopEmpty      , "empty"        ,Empty   ,None ,fnopLeaf)
  case knopEmpty:
    return CreateNodeT<knopEmpty>(pnode->ichMin,pnode->ichLim);
      // Unary operators.
      //PTNODE(knopNot        , "~"            ,BitNot  ,Uni  ,fnopUni)
      //PTNODE(knopNeg        , "unary -"    ,Neg     ,Uni  ,fnopUni)
      //PTNODE(knopPos        , "unary +"    ,Pos     ,Uni  ,fnopUni)
      //PTNODE(knopLogNot     , "!"            ,LogNot  ,Uni  ,fnopUni)
      //PTNODE(knopEllipsis     , "..."       ,Spread  ,Uni    , fnopUni)
      //PTNODE(knopDecPost    , "-- post"    ,Dec     ,Uni  ,fnopUni|fnopAsg)
      //PTNODE(knopIncPre     , "++ pre"    ,Inc     ,Uni  ,fnopUni|fnopAsg)
      //PTNODE(knopDecPre     , "-- pre"    ,Dec     ,Uni  ,fnopUni|fnopAsg)
      //PTNODE(knopTypeof     , "typeof"    ,None    ,Uni  ,fnopUni)
      //PTNODE(knopVoid       , "void"        ,Void    ,Uni  ,fnopUni)
      //PTNODE(knopDelete     , "delete"    ,None    ,Uni  ,fnopUni)
  case knopNot:
  case knopNeg:
  case knopPos:
  case knopLogNot:
  case knopEllipsis:
  case knopIncPost:
  case knopDecPost:
  case knopIncPre:
  case knopDecPre:
  case knopTypeof:
  case knopVoid:
  case knopDelete:
    return CreateUniNode(pnode->nop,CopyPnode(pnode->sxUni.pnode1),pnode->ichMin,pnode->ichLim);
      //PTNODE(knopArray      , "arr cnst"    ,None    ,Uni  ,fnopUni)
      //PTNODE(knopObject     , "obj cnst"    ,None    ,Uni  ,fnopUni)
  case knopArray:
  case knopObject:
    // TODO: need to copy arr
    Assert(false);
    break;
      // Binary operators
      //PTNODE(knopAdd        , "+"            ,Add     ,Bin  ,fnopBin)
      //PTNODE(knopSub        , "-"            ,Sub     ,Bin  ,fnopBin)
      //PTNODE(knopMul        , "*"            ,Mul     ,Bin  ,fnopBin)
      //PTNODE(knopExpo       , "**"           ,Expo     ,Bin  ,fnopBin)
      //PTNODE(knopDiv        , "/"            ,Div     ,Bin  ,fnopBin)
      //PTNODE(knopMod        , "%"            ,Mod     ,Bin  ,fnopBin)
      //PTNODE(knopOr         , "|"            ,BitOr   ,Bin  ,fnopBin)
      //PTNODE(knopXor        , "^"            ,BitXor  ,Bin  ,fnopBin)
      //PTNODE(knopAnd        , "&"            ,BitAnd  ,Bin  ,fnopBin)
      //PTNODE(knopEq         , "=="        ,EQ      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopNe         , "!="        ,NE      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopLt         , "<"            ,LT      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopLe         , "<="        ,LE      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopGe         , ">="        ,GE      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopGt         , ">"            ,GT      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopEqv        , "==="        ,Eqv     ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopIn         , "in"        ,In      ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopInstOf     , "instanceof",InstOf  ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopNEqv       , "!=="        ,NEqv    ,Bin  ,fnopBin|fnopRel)
      //PTNODE(knopComma      , ","            ,None    ,Bin  ,fnopBin)
      //PTNODE(knopLogOr      , "||"        ,None    ,Bin  ,fnopBin)
      //PTNODE(knopLogAnd     , "&&"        ,None    ,Bin  ,fnopBin)
      //PTNODE(knopLsh        , "<<"        ,Lsh     ,Bin  ,fnopBin)
      //PTNODE(knopRsh        , ">>"        ,Rsh     ,Bin  ,fnopBin)
      //PTNODE(knopRs2        , ">>>"        ,Rs2     ,Bin  ,fnopBin)
  case knopAdd:
  case knopSub:
  case knopMul:
  case knopExpo:
  case knopDiv:
  case knopMod:
  case knopOr:
  case knopXor:
  case knopAnd:
  case knopEq:
  case knopNe:
  case knopLt:
  case knopLe:
  case knopGe:
  case knopGt:
  case knopEqv:
  case knopIn:
  case knopInstOf:
  case knopNEqv:
  case knopComma:
  case knopLogOr:
  case knopLogAnd:
  case knopLsh:
  case knopRsh:
  case knopRs2:
      //PTNODE(knopAsg        , "="            ,None    ,Bin  ,fnopBin|fnopAsg)
  case knopAsg:
      //PTNODE(knopDot        , "."            ,None    ,Bin  ,fnopBin)
  case knopDot:
      //PTNODE(knopAsgAdd     , "+="        ,Add     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgAdd:
      //PTNODE(knopAsgSub     , "-="        ,Sub     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgSub:
      //PTNODE(knopAsgMul     , "*="        ,Mul     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgMul:
      //PTNODE(knopAsgDiv     , "/="        ,Div     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgExpo:
      //PTNODE(knopAsgExpo    , "**="       ,Expo    ,Bin  ,fnopBin|fnopAsg)
  case knopAsgDiv:
      //PTNODE(knopAsgMod     , "%="        ,Mod     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgMod:
      //PTNODE(knopAsgAnd     , "&="        ,BitAnd  ,Bin  ,fnopBin|fnopAsg)
  case knopAsgAnd:
      //PTNODE(knopAsgXor     , "^="        ,BitXor  ,Bin  ,fnopBin|fnopAsg)
  case knopAsgXor:
      //PTNODE(knopAsgOr      , "|="        ,BitOr   ,Bin  ,fnopBin|fnopAsg)
  case knopAsgOr:
      //PTNODE(knopAsgLsh     , "<<="        ,Lsh     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgLsh:
      //PTNODE(knopAsgRsh     , ">>="        ,Rsh     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgRsh:
      //PTNODE(knopAsgRs2     , ">>>="        ,Rs2     ,Bin  ,fnopBin|fnopAsg)
  case knopAsgRs2:
      //PTNODE(knopMember     , ":"            ,None    ,Bin  ,fnopBin)
  case knopMember:
  case knopMemberShort:
      //PTNODE(knopIndex      , "[]"        ,None    ,Bin  ,fnopBin)
      //PTNODE(knopList       , "<list>"    ,None    ,Bin  ,fnopNone)

  case knopIndex:
  case knopList:
    return CreateBinNode(pnode->nop,CopyPnode(pnode->sxBin.pnode1),
                         CopyPnode(pnode->sxBin.pnode2),pnode->ichMin,pnode->ichLim);

      //PTNODE(knopCall       , "()"        ,None    ,Bin  ,fnopBin)
      //PTNODE(knopNew        , "new"        ,None    ,Bin  ,fnopBin)
  case knopNew:
  case knopCall:
    return CreateCallNode(pnode->nop,CopyPnode(pnode->sxCall.pnodeTarget),
                         CopyPnode(pnode->sxCall.pnodeArgs),pnode->ichMin,pnode->ichLim);
      //PTNODE(knopQmark      , "?"            ,None    ,Tri  ,fnopBin)
  case knopQmark:
    return CreateTriNode(pnode->nop,CopyPnode(pnode->sxTri.pnode1),
                         CopyPnode(pnode->sxTri.pnode2),CopyPnode(pnode->sxTri.pnode3),
                         pnode->ichMin,pnode->ichLim);
      // General nodes.
      //PTNODE(knopVarDecl    , "varDcl"    ,None    ,Var  ,fnopNone)
    case knopVarDecl: {
      ParseNode* copyNode=CreateNodeT<knopVarDecl>(pnode->ichMin,pnode->ichLim);
      copyNode->sxVar.pnodeInit=CopyPnode(pnode->sxVar.pnodeInit);
      copyNode->sxVar.sym=pnode->sxVar.sym;
      // TODO: mult-decl
      Assert(pnode->sxVar.pnodeNext==NULL);
      copyNode->sxVar.pnodeNext=NULL;
      return copyNode;
    }
      //PTNODE(knopFncDecl    , "fncDcl"    ,None    ,Fnc  ,fnopLeaf)
      //PTNODE(knopProg       , "program"    ,None    ,Fnc  ,fnopNone)
  case knopFncDecl:
  case knopProg:
    Assert(false);
    break;
      //PTNODE(knopEndCode    , "<endcode>"    ,None    ,None ,fnopNone)
  case knopEndCode:
    break;
      //PTNODE(knopDebugger   , "debugger"    ,None    ,None ,fnopNone)
  case knopDebugger:
    break;
      //PTNODE(knopFor        , "for"        ,None    ,For  ,fnopBreak|fnopContinue)
    case knopFor: {
      ParseNode* copyNode=CreateNodeT<knopFor>(pnode->ichMin,pnode->ichLim);
      copyNode->sxFor.pnodeInverted=NULL;
      copyNode->sxFor.pnodeInit=CopyPnode(pnode->sxFor.pnodeInit);
      copyNode->sxFor.pnodeCond=CopyPnode(pnode->sxFor.pnodeCond);
      copyNode->sxFor.pnodeIncr=CopyPnode(pnode->sxFor.pnodeIncr);
      copyNode->sxFor.pnodeBody=CopyPnode(pnode->sxFor.pnodeBody);
      return copyNode;
    }
      //PTNODE(knopIf         , "if"        ,None    ,If   ,fnopNone)
  case knopIf:
    Assert(false);
    break;
      //PTNODE(knopWhile      , "while"        ,None    ,While,fnopBreak|fnopContinue)
  case knopWhile:
    Assert(false);
    break;
      //PTNODE(knopDoWhile    , "do-while"    ,None    ,While,fnopBreak|fnopContinue)
  case knopDoWhile:
    Assert(false);
    break;
      //PTNODE(knopForIn      , "for in"    ,None    ,ForIn,fnopBreak|fnopContinue|fnopCleanup)
  case knopForIn:
    Assert(false);
    break;
  case knopForOf:
    Assert(false);
    break;
      //PTNODE(knopReturn     , "return"    ,None    ,Uni  ,fnopNone)
  case knopReturn: {
    ParseNode* copyNode=CreateNodeT<knopReturn>(pnode->ichMin,pnode->ichLim);
    copyNode->sxReturn.pnodeExpr=CopyPnode(pnode->sxReturn.pnodeExpr);
    return copyNode;
  }
      //PTNODE(knopBlock      , "{}"        ,None    ,Block,fnopNone)
  case knopBlock: {
    ParseNode* copyNode=CreateBlockNode(pnode->ichMin,pnode->ichLim,pnode->sxBlock.blockType);
    if (pnode->grfpn & PNodeFlags::fpnSyntheticNode) {
        // fpnSyntheticNode is sometimes set on PnodeBlockType::Regular blocks which
        // CreateBlockNode() will not automatically set for us, so set it here if it's
        // specified on the source node.
        copyNode->grfpn |= PNodeFlags::fpnSyntheticNode;
    }
    copyNode->sxBlock.pnodeStmt=CopyPnode(pnode->sxBlock.pnodeStmt);
    return copyNode;
  }
      //PTNODE(knopWith       , "with"        ,None    ,With ,fnopCleanup)
  case knopWith:
    Assert(false);
    break;
      //PTNODE(knopBreak      , "break"        ,None    ,Jump ,fnopNone)
  case knopBreak:
    Assert(false);
    break;
      //PTNODE(knopContinue   , "continue"    ,None    ,Jump ,fnopNone)
  case knopContinue:
    Assert(false);
    break;
      //PTNODE(knopLabel      , "label"        ,None    ,Label,fnopNone)
  case knopLabel:
    Assert(false);
    break;
      //PTNODE(knopSwitch     , "switch"    ,None    ,Switch,fnopBreak)
  case knopSwitch:
    Assert(false);
    break;
      //PTNODE(knopCase       , "case"        ,None    ,Case ,fnopNone)
  case knopCase:
    Assert(false);
    break;
      //PTNODE(knopTryFinally,"try-finally",None,TryFinally,fnopCleanup)
  case knopTryFinally:
    Assert(false);
    break;
  case knopFinally:
    Assert(false);
    break;
      //PTNODE(knopCatch      , "catch"     ,None    ,Catch,fnopNone)
  case knopCatch:
    Assert(false);
    break;
      //PTNODE(knopTryCatch      , "try-catch" ,None    ,TryCatch  ,fnopCleanup)
  case knopTryCatch:
    Assert(false);
    break;
      //PTNODE(knopTry        , "try"       ,None    ,Try  ,fnopCleanup)
  case knopTry:
    Assert(false);
    break;
      //PTNODE(knopThrow      , "throw"     ,None    ,Uni  ,fnopNone)
  case knopThrow:
    Assert(false);
    break;
  default:
    Assert(false);
    break;
    }
    return NULL;
}