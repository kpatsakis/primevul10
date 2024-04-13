Jsi_RC jsi_evalcode_sub(jsi_Pstate *ps, Jsi_OpCodes *opcodes, 
     jsi_ScopeChain *scope, Jsi_Value *currentScope,
     Jsi_Value *_this, Jsi_Value *vret)
{
    Jsi_Interp* interp = ps->interp;
    jsi_OpCode *ip = &opcodes->codes[0];
    Jsi_RC rc = JSI_OK;
    int curLine = 0;
    int context_id = ps->_context_id++, lop = -1;
    jsi_OpCode *end = &opcodes->codes[opcodes->code_len];
    jsi_TryList  *trylist = NULL;
    bool strict = interp->strict;
    const char *curFile = NULL;
    
    if (currentScope->vt != JSI_VT_OBJECT) {
        Jsi_LogBug("Eval: current scope is not a object");
        return JSI_ERROR;
    }
    if (interp->maxDepth>0 && interp->level > interp->maxDepth)
        rc = Jsi_LogError("Exceeded call depth: %d", interp->level);
    
    while(ip < end && rc == JSI_OK) {
        int plop = ip->op;

        if (ip->logflag) { // Mask out LogDebug, etc if not enabled.
            interp->curIp = ip;
            switch (ip->logflag) {
                case jsi_Oplf_assert:
                    if (!interp->asserts) {
                        ip++;
                        if (ip->logflag != jsi_Oplf_assert && (ip->op == OP_POP || ip->op == OP_RET))
                            ip++;
                        continue;
                    }
                    break;
                case jsi_Oplf_debug:
                    if (!interp->logOpts.Debug && !(interp->framePtr->logflag &(1<<jsi_Oplf_debug))) {
                        ip++;
                        if (ip->logflag != jsi_Oplf_debug && (ip->op == OP_POP || ip->op == OP_RET))
                            ip++;
                        continue;
                    }
                    break;
                case jsi_Oplf_test:
                    if (!interp->logOpts.Test && !(interp->framePtr->logflag &(1<<jsi_Oplf_test))) {
                        ip++;
                        if (ip->logflag != jsi_Oplf_test && (ip->op == OP_POP || ip->op == OP_RET))
                            ip++;
                        continue;
                    }
                    break;
                case jsi_Oplf_trace:
                    if (!interp->logOpts.Trace && !(interp->framePtr->logflag &(1<<jsi_Oplf_trace))) {
                        ip++;
                        if (ip->logflag != jsi_Oplf_trace && (ip->op == OP_POP || ip->op == OP_RET))
                            ip++;
                        continue;
                    }
                    break;
                default:
                    break;
            }
        }
        if (interp->interrupted) {
            if (!interp->framePtr->tryDepth) {
                Jsi_LogError("program interrupted: function=%s", interp->framePtr->funcName);
                interp->interrupted = 0;
            } else {
                interp->interrupted++;
            }
            rc = JSI_ERROR;
            break;
        }
        if (interp->exited) {
            rc = JSI_ERROR;
            break;
        }
        interp->opCnt++;
        if (interp->maxOpCnt && interp->opCnt > interp->maxOpCnt) {
            puts("EXEC CAP EXCEED");
            interp->maxOpCnt += 1000;
            rc = Jsi_LogError("Exceeded execution cap: %d", interp->opCnt);
            interp->exited = 1;
            interp->exitCode = 99;
            break;
        }
        if (interp->traceOp) {
            jsiDumpInstr(interp, ps, _this, trylist, ip, opcodes);
        }
        if (interp->parent && interp->busyCallback && (interp->opCnt%(interp->busyInterval<=0?100000:interp->busyInterval))==0) {
            // Handle parent interp events.
            if (jsi_FuncIsNoop(interp, interp->busyCallback))
                Jsi_EventProcess(interp->parent, -1);
            else {
                Jsi_DString nStr;
                Jsi_DSInit(&nStr);
                Jsi_DSPrintf(&nStr, "[\"#Interp_%d\", %d]", interp->objId, interp->opCnt);//TODO: use actual time interval rather than opCnt.
                if (Jsi_FunctionInvokeJSON(interp->parent, interp->busyCallback, Jsi_DSValue(&nStr), NULL) != JSI_OK)
                    rc = JSI_ERROR;
                Jsi_DSFree(&nStr);
            }
        }
        ip->hit=1;
#ifndef USE_STATIC_STACK
        if ((interp->maxStack-interp->framePtr->Sp)<STACK_MIN_PAD)
            jsiSetupStack(interp);
#endif
        jsiPush(interp,0);
        interp->curIp = ip;
        // Carry forward line/file info from previous OPs.
        if (!ip->Line)
            ip->Line = curLine;
        else
            curLine = ip->Line;
        if (!ip->fname)
            ip->fname = curFile;
        else
            curFile = ip->fname;
        if (interp->debugOpts.hook) {
            interp->framePtr->fileName = curFile;
            interp->framePtr->line = curLine;
            if ((rc = (*interp->debugOpts.hook)(interp, curFile, curLine, interp->framePtr->level, interp->curFunction, jsi_opcode_string(ip->op), ip, NULL)) != JSI_OK)
                break;
        }

        switch(ip->op) {
            case OP_NOP:
            case OP_LASTOP:
                break;
            case OP_PUSHUND:
                Jsi_ValueMakeUndef(interp, &_jsi_STACKIDX(interp->framePtr->Sp));
                jsiPush(interp,1);
                break;
            case OP_PUSHNULL:
                Jsi_ValueMakeNull(interp, &_jsi_STACKIDX(interp->framePtr->Sp));
                jsiPush(interp,1);
                break;
            case OP_PUSHBOO:
                Jsi_ValueMakeBool(interp, &_jsi_STACKIDX(interp->framePtr->Sp), (uintptr_t)ip->data);
                jsiPush(interp,1);
                break;
            case OP_PUSHNUM:
                Jsi_ValueMakeNumber(interp, &_jsi_STACKIDX(interp->framePtr->Sp), (*((Jsi_Number *)ip->data)));
                jsiPush(interp,1);
                break;
            case OP_PUSHSTR: {
                Jsi_Value **v = &_jsi_STACKIDX(interp->framePtr->Sp);
                Jsi_ValueMakeStringKey(interp, v, (char*)ip->data);
                interp->lastPushStr = Jsi_ValueString(interp, *v, NULL);
                jsiPush(interp,1);
                break;
            }
            case OP_PUSHVSTR: {
                Jsi_String *s = (Jsi_String *)ip->data;
                Jsi_Value **v = &_jsi_STACKIDX(interp->framePtr->Sp);
                if (s->flags&1)
                    jsi_ValueMakeBlobDup(interp,v, (uchar*)s->str, s->len);
                else {
                    Jsi_ValueMakeStringKey(interp, v, s->str);
                    interp->lastPushStr = s->str;
                }
                jsiPush(interp,1);
                break;
            }
            case OP_PUSHVAR: {
                rc = jsi_PushVar(ps, ip, scope, currentScope, context_id);      
                break;
            }
            case OP_PUSHFUN: {
                jsi_PushFunc(ps, ip, scope, currentScope);
                break;
            }
            case OP_NEWFCALL:
                if (interp->maxUserObjs && interp->userObjCnt > interp->maxUserObjs) {
                    rc = Jsi_LogError("Max 'new' count exceeded");
                    break;
                }
            case OP_FCALL: {
                /* TODO: need reliable way to capture func string name to handle unknown functions.*/
                int discard = ((ip+1)<end && ip[1].op == OP_POP);
                switch (jsiEvalFunction(ps, ip, discard)) {        /* throw an execption */
                    case JSI_OK: break;
                    case JSI_BREAK:
                        if (interp->framePtr->tryDepth<=0)
                            interp->isHelp = 1;
                        JSI_DO_THROW("help");
                        break;
                    default:  
                        JSI_DO_THROW("fcall");
                }
                strict = interp->strict;
                /* TODO: new Function return a function without scopechain, add here */
                break;
            }
            case OP_SUBSCRIPT: {
                rc = jsi_evalSubscript(interp, _jsi_TOQ, _jsi_TOP, ip, end, currentScope);
                break;
            }
            case OP_ASSIGN: {
                Jsi_Value *sval = _jsi_TOP, *dval = _jsi_TOQ;
                bool globThis = (sval->vt == JSI_VT_OBJECT && sval->d.obj == interp->csc->d.obj);
                if ((uintptr_t)ip->data == 1) {
                    jsiVarDeref(interp,1);
                    rc = jsi_ValueAssign(interp, dval, sval, lop);                    
                    if (strict && sval->vt == JSI_VT_UNDEF)
                        rc = jsi_ValueAssignCheck(interp, sval, lop);
                    jsiPop(interp,1);
                } else {
                    jsiVarDeref(interp, 3);
                    Jsi_Value *v3 = _jsi_STACKIDX(interp->framePtr->Sp-3);
                    if (v3->vt == JSI_VT_OBJECT) {
                        if (strict && sval->vt == JSI_VT_UNDEF)
                            rc = jsi_ValueAssignCheck(interp, sval, lop);
                        jsi_ValueObjKeyAssign(interp, v3, dval, sval, 0);
                        jsi_ValueDebugLabel(sval, "assign", NULL);
                    } else if (strict)
                        rc = Jsi_LogError("assign to a non-exist object");
                    jsiClearStack(interp,3);
                    Jsi_ValueCopy(interp,v3, sval);
                    dval = v3;
                    jsiPop(interp, 2);
                }
                if (interp->framePtr->level<=1 && globThis && rc == JSI_OK && dval && dval->vt == JSI_VT_VARIABLE) {
                    dval = dval->d.lval;
                    //printf("GLOBAL THIS: %p\n", dval);
                    Jsi_HashSet(interp->genValueTbl, dval, dval);
                }
                break;
            }
            case OP_PUSHREG: {
                Jsi_Obj *obj = Jsi_ObjNewType(interp, JSI_OT_REGEXP);
                obj->d.robj = (Jsi_Regex *)ip->data;
                Jsi_ValueMakeObject(interp, &_jsi_STACKIDX(interp->framePtr->Sp), obj);
                jsiPush(interp,1);
                break;
            }
            case OP_PUSHARG:
                //Jsi_ValueCopy(interp,_jsi_STACKIDX(interp->framePtr->Sp), currentScope);
                
                if (!interp->framePtr->arguments) {
                    interp->framePtr->arguments = Jsi_ValueNewObj(interp,
                        Jsi_ObjNewArray(interp, currentScope->d.obj->arr, currentScope->d.obj->arrCnt, 0));
                    Jsi_IncrRefCount(interp, interp->framePtr->arguments);
                    if (interp->hasCallee) {
                        Jsi_Value *callee = Jsi_ValueObjLookup(interp, currentScope, "\1callee\1", 0);
                        if (callee)
                            Jsi_ValueInsert(interp, interp->framePtr->arguments, "\1callee\1", callee, JSI_OM_DONTENUM);
                    }
                    // interp->framePtr->arguments->d.obj->__proto__ = interp->Object_prototype; // ecma
                }
                Jsi_ValueCopy(interp,_jsi_STACKIDX(interp->framePtr->Sp), interp->framePtr->arguments);
                jsiPush(interp,1);
                break;
            case OP_PUSHTHS: { //TODO: Value copy can cause memory leak!
                Jsi_Value *tval = _jsi_STACKIDX(interp->framePtr->Sp);
                Jsi_ValueCopy(interp, tval, _this);
                /*if (interp->csc == _this)
                    Jsi_ValueDup2(interp, &tval, _this);
                else
                    Jsi_ValueCopy(interp, tval, _this);*/
                jsiPush(interp,1);
                break;
            }
            case OP_PUSHTOP:
                Jsi_ValueCopy(interp,_jsi_STACKIDX(interp->framePtr->Sp), _jsi_TOP);
                jsiPush(interp,1);
                break;
            case OP_UNREF:
                jsiVarDeref(interp,1);
                break;
            case OP_PUSHTOP2: {
                Jsi_Value *vp1 = _jsi_STACKIDX(interp->framePtr->Sp);
                Jsi_Value *vp2 = _jsi_STACKIDX(interp->framePtr->Sp+1);
                if (!vp1 || !vp2)
                    rc = Jsi_LogError("Invalid lookup/push");
                else {
                    Jsi_ValueCopy(interp, vp1, _jsi_TOQ);
                    Jsi_ValueCopy(interp, vp2, _jsi_TOP);
                    jsiPush(interp, 2);
                }
                break;
            }
            case OP_CHTHIS: {
                if (ip->data) {
                    int t = interp->framePtr->Sp - 2;
                    Assert(t>=0);
                    Jsi_Value *v = _jsi_THISIDX(t);
                    jsiClearThis(interp, t);
                    Jsi_ValueCopy(interp, v, _jsi_TOQ);
                    if (v->vt == JSI_VT_VARIABLE) {
                        Jsi_ValueCopy(interp, v, v->d.lval);
                    }
                    Jsi_ValueToObject(interp, v);
                }
                break;
            }
            case OP_LOCAL: {
                Jsi_Value key = VALINIT, *kPtr = &key; // Note we use a string key so no reset needed.
                Jsi_ValueMakeStringKey(interp, &kPtr, (char*)ip->data);
                jsi_ValueObjKeyAssign(interp, currentScope, kPtr, NULL, JSI_OM_DONTENUM);
                context_id = ps->_context_id++;
                break;
            }
            case OP_POP: {
                Jsi_Value *tval = _jsi_TOP;
                if ((interp->evalFlags&JSI_EVAL_RETURN) && (ip+1) >= end && 
                (Jsi_ValueIsObjType(interp, tval, JSI_OT_ITER)==0 &&
                Jsi_ValueIsObjType(interp, tval, JSI_OT_FUNCTION)==0)) {
                    /* Interactive and last instruction is a pop: save result. */
                    Jsi_ValueMove(interp, vret, tval); /*TODO***: correct ***/
                    tval->vt = JSI_VT_UNDEF;
                }
                jsiPop(interp, (uintptr_t)ip->data);
                break;
            }
            case OP_NEG:
                jsiVarDeref(interp,1);
                Jsi_ValueToNumber(interp, _jsi_TOP);
                rc = _jsi_StrictChk(_jsi_TOP);
                _jsi_TOP->d.num = -(_jsi_TOP->d.num);
                break;
            case OP_POS:
                jsiVarDeref(interp,1);
                Jsi_ValueToNumber(interp, _jsi_TOP);
                rc = _jsi_StrictChk(_jsi_TOP);
                break;
            case OP_NOT: {
                int val = 0;
                jsiVarDeref(interp,1);
                
                val = Jsi_ValueIsTrue(interp, _jsi_TOP);
                
                jsiClearStack(interp,1);
                Jsi_ValueMakeBool(interp, &_jsi_TOP, !val);
                break;
            }
            case OP_BNOT: {
                jsiVarDeref(interp,1);
                jsi_ValueToOInt32(interp, _jsi_TOP);
                rc = _jsi_StrictChk(_jsi_TOP);
                _jsi_TOP->d.num = (Jsi_Number)(~((int)_jsi_TOP->d.num));
                break;
            }
            case OP_ADD: {
                jsiVarDeref(interp,2);
                Jsi_Value *v, *v1 = _jsi_TOP, *v2 = _jsi_TOQ;
                int l1, l2;
                if (strict)
                    if (Jsi_ValueIsUndef(interp, v1) || Jsi_ValueIsUndef(interp, v2)) {
                        rc = Jsi_LogError("operand value to + is undefined");
                        break;
                    }
                char *s1 = Jsi_ValueString(interp, v1, &l1);
                char *s2 = Jsi_ValueString(interp, v2, &l2);
                if (s1 || s2) {
                    char *str;
                    if (!(s1 && s2)) {
                        v = (s1 ? v2 : v1);
                        jsi_ValueToPrimitive(interp, &v);
                        Jsi_ValueToString(interp, v, NULL);
                        str = Jsi_ValueString(interp, v, (s1?&l2:&l1));
                        if (s1) s2 = str; else s1 = str;
                    }
                    Assert(l1>=0 && l1<=JSI_MAX_ALLOC_BUF);
                    Assert(l2>=0 && l2<=JSI_MAX_ALLOC_BUF);
                    str = (char*)Jsi_Malloc(l1+l2+1);
                    memcpy(str, s2, l2);
                    memcpy(str+l2, s1, l1);
                    str[l1+l2] = 0;
                    jsiClearStack(interp,2);
                    Jsi_ValueMakeBlob(interp, &v2, (uchar*)str, l1+l2);
                } else {
                    Jsi_ValueToNumber(interp, v1);
                    Jsi_ValueToNumber(interp, v2);
                    rc = _jsi_StrictChk2(v1, v2);
                    Jsi_Number n = v1->d.num + v2->d.num;
                    jsiClearStack(interp,2);
                    Jsi_ValueMakeNumber(interp, &v2, n);
                }
                jsiPop(interp,1);
                break;
            }
            case OP_IN: {
                Jsi_Value *v, *vl;
                const char *cp = NULL;
                Jsi_Number nval;
                jsiVarDeref(interp,2);
                vl = _jsi_TOQ;
                v = _jsi_TOP;
                if (Jsi_ValueIsString(interp,vl))
                    cp = Jsi_ValueGetStringLen(interp, vl, NULL);
                else if (Jsi_ValueIsNumber(interp,vl))
                    Jsi_ValueGetNumber(interp, vl, &nval);
                else {
                    if (strict)
                        Jsi_LogWarn("expected string or number before IN");
                    Jsi_ValueMakeBool(interp, &_jsi_TOQ, 0);
                    jsiPop(interp,1);
                    break;
                }
                
                if (v->vt == JSI_VT_VARIABLE) {
                    v = v->d.lval;
                    SIGASSERT(v, VALUE);
                }
                if (v->vt != JSI_VT_OBJECT || v->d.obj->ot != JSI_OT_OBJECT) {
                    if (strict)
                        Jsi_LogWarn("expected object after IN");
                    Jsi_ValueMakeBool(interp, &_jsi_TOQ, 0);
                    jsiPop(interp,1);
                    break;
                }
                int bval = 0;
                char nbuf[JSI_MAX_NUMBER_STRING];
                Jsi_Value *vv;
                Jsi_Obj *obj = v->d.obj;
                if (!cp) {
                    snprintf(nbuf, sizeof(nbuf), "%d", (int)nval);
                    cp = nbuf;
                }
                if (obj->arr) {
                    vv = jsi_ObjArrayLookup(interp, obj, (char*)cp);
                } else {
                    vv = Jsi_TreeObjGetValue(obj, (char*)cp, 1);
                }
                bval = (vv != 0);
                Jsi_ValueMakeBool(interp, &_jsi_TOQ, bval);
                jsiPop(interp,1);
                break;
            }
            case OP_SUB: 
                common_math_opr(-); break;
            case OP_MUL:
                common_math_opr(*); break;
            case OP_DIV:
                common_math_opr(/); break;
            case OP_MOD: {
                jsiVarDeref(interp,2);
                if (!Jsi_ValueIsType(interp,_jsi_TOP, JSI_VT_NUMBER))
                    Jsi_ValueToNumber(interp, _jsi_TOP);
                if (!Jsi_ValueIsType(interp,_jsi_TOQ, JSI_VT_NUMBER))
                    Jsi_ValueToNumber(interp, _jsi_TOQ);
                rc = _jsi_StrictChk2(_jsi_TOP,_jsi_TOQ);
                if (rc == JSI_OK)
                    _jsi_TOQ->d.num = fmod(_jsi_TOQ->d.num, _jsi_TOP->d.num);
                jsiPop(interp,1);
                break;
            }
            case OP_LESS:
                jsiVarDeref(interp,2);
                rc = jsi_logic_less(interp,2,1);
                jsiPop(interp,1);
                break;
            case OP_GREATER:
                jsiVarDeref(interp,2);
                rc = jsi_logic_less(interp,1,2);
                jsiPop(interp,1);
                break;
            case OP_LESSEQU:
                jsiVarDeref(interp,2);
                rc = jsi_logic_less(interp,1,2);
                if (rc == JSI_OK)
                    _jsi_TOQ->d.val = !_jsi_TOQ->d.val;
                jsiPop(interp,1);
                break;
            case OP_GREATEREQU:
                jsiVarDeref(interp,2);
                rc = jsi_logic_less(interp,2,1);
                if (rc == JSI_OK)
                    _jsi_TOQ->d.val = !_jsi_TOQ->d.val;
                jsiPop(interp,1);
                break;
            case OP_EQUAL:
            case OP_NOTEQUAL: {
                jsiVarDeref(interp,2);
                int r = Jsi_ValueCmp(interp, _jsi_TOP, _jsi_TOQ, 0);
                r = (ip->op == OP_EQUAL ? !r : r);
                jsiClearStack(interp,2);
                Jsi_ValueMakeBool(interp, &_jsi_TOQ, r);
                jsiPop(interp,1);
                break;
            }
            case OP_STRICTEQU:
            case OP_STRICTNEQ: {
                int r = 0;
                jsiVarDeref(interp,2);
                rc = _jsi_StrictUChk3(_jsi_TOQ, _jsi_TOP);
                r = !Jsi_ValueIsEqual(interp, _jsi_TOP, _jsi_TOQ);
                r = (ip->op == OP_STRICTEQU ? !r : r);
                jsiClearStack(interp,2);
                Jsi_ValueMakeBool(interp, &_jsi_TOQ, r);
                jsiPop(interp,1);
                break;
            }
            case OP_BAND: 
                common_bitwise_opr(&); break;
            case OP_BOR:
                common_bitwise_opr(|); break;
            case OP_BXOR:
                common_bitwise_opr(^); break;
            case OP_SHF: {
                jsiVarDeref(interp,2);
                jsi_ValueToOInt32(interp, _jsi_TOQ);
                jsi_ValueToOInt32(interp, _jsi_TOP);
                int t1 = (int)_jsi_TOQ->d.num;
                int t2 = ((unsigned int)_jsi_TOP->d.num) & 0x1f;
                if (ip->data) {                 /* shift right */
                    if ((uintptr_t)ip->data == 2) {   /* unsigned shift */
                        unsigned int t3 = (unsigned int)t1;
                        t3 >>= t2;
                        Jsi_ValueMakeNumber(interp, &_jsi_TOQ, t3);
                    } else {
                        t1 >>= t2;
                        Jsi_ValueMakeNumber(interp, &_jsi_TOQ, t1);
                    }
                } else {
                    t1 <<= t2;
                    Jsi_ValueMakeNumber(interp, &_jsi_TOQ, t1);
                }
                jsiPop(interp,1);
                break;
            }
            case OP_KEY: {
                jsiVarDeref(interp,1);
                if (ip->isof && !Jsi_ValueIsArray(interp, _jsi_TOP)) {
                    rc = Jsi_LogError("operand not an array");
                    break;
                }
                if (_jsi_TOP->vt != JSI_VT_UNDEF && _jsi_TOP->vt != JSI_VT_NULL)
                    Jsi_ValueToObject(interp, _jsi_TOP);
                Jsi_Value *spret = Jsi_ValueNew1(interp);
                jsi_ValueObjGetKeys(interp, _jsi_TOP, spret, ip->isof);
                Jsi_ValueReplace(interp, _jsi_STACK+interp->framePtr->Sp, spret);  
                Jsi_DecrRefCount(interp, spret);  
                jsiPush(interp,1);
                break;
            }
            case OP_NEXT: {
                Jsi_Value *toq = _jsi_TOQ, *top = _jsi_TOP;
                if (toq->vt != JSI_VT_OBJECT || toq->d.obj->ot != JSI_OT_ITER) Jsi_LogBug("next: toq not a iter\n");
                if (top->vt != JSI_VT_VARIABLE) {
                    rc = Jsi_LogError ("invalid for/in left hand-side");
                    break;
                }
                if (strict && top->f.bits.local==0) {
                    const char *varname = "";
                    Jsi_Value *v = top->d.lval;
                    if (v->f.bits.lookupfailed)
                        varname = v->d.lookupFail;

                    rc = Jsi_LogError("function created global: \"%s\"", varname);
                    break;
                }
                
                Jsi_IterObj *io = toq->d.obj->d.iobj;
                if (io->iterCmd) {
                    io->iterCmd(io, top, _jsi_STACKIDX(interp->framePtr->Sp-3), io->iter++);
                } else {
                    while (io->iter < io->count) {
                        if (!io->isArrayList) {
                            if (Jsi_ValueKeyPresent(interp, _jsi_STACKIDX(interp->framePtr->Sp-3), io->keys[io->iter],1)) 
                                break;
                        } else {
                            while (io->cur < io->obj->arrCnt) {
                                if (io->obj->arr[io->cur]) break;
                                io->cur++;
                            }
                            if (io->cur >= io->obj->arrCnt) {
                                /* TODO: Is this really a bug??? */
                                /* Jsi_LogBug("NOT FOUND LIST ARRAY");*/
                                io->iter = io->count;
                                break;
                            } else if (io->obj->arr[io->cur]) {
                                io->cur++;
                                break;
                            }
                        }
                        io->iter++;
                    }
                    if (io->iter >= io->count) {
                        jsiClearStack(interp,1);
                        Jsi_ValueMakeNumber(interp, &_jsi_TOP, 0);
                    } else {
                        Jsi_Value **vPtr = &_jsi_TOP->d.lval, *v = *vPtr;
                        SIGASSERT(v, VALUE);
                        Jsi_ValueReset(interp, vPtr);
                        if (io->isArrayList) {
                            if (!io->isof)
                                Jsi_ValueMakeNumber(interp, &v, io->cur-1);
                            else if (!io->obj->arr[io->cur-1])
                                Jsi_ValueMakeNull(interp, &v);
                            else
                                Jsi_ValueCopy(interp, v, io->obj->arr[io->cur-1]);
                        } else
                            Jsi_ValueMakeStringKey(interp, &v, io->keys[io->iter]);
                        io->iter++;
                        
                        jsiClearStack(interp,1);
                        Jsi_ValueMakeNumber(interp, &_jsi_TOP, 1);
                    }
                    break;
                }
            }
            case OP_INC:
            case OP_DEC: {
                int inc = ip->op == OP_INC ? 1 : -1;
                
                if (_jsi_TOP->vt != JSI_VT_VARIABLE) {
                    rc = Jsi_LogError("operand not left value");
                    break;
                }
                Jsi_Value *v = _jsi_TOP->d.lval;
                SIGASSERT(v, VALUE);
                Jsi_ValueToNumber(interp, v);
                rc = _jsi_StrictChk(v);
                v->d.num += inc;
                    
                jsiVarDeref(interp,1);
                if (ip->data) {
                    _jsi_TOP->d.num -= inc;
                }
                break;
            }
            case OP_TYPEOF: {
                const char *typ;
                Jsi_Value *v = _jsi_TOP;
                if (v->vt == JSI_VT_VARIABLE) {
                    v = v->d.lval;
                    SIGASSERT(v, VALUE);
                }
                typ = Jsi_ValueTypeStr(interp, v);
                jsiVarDeref(interp,1);
                Jsi_ValueMakeStringKey(interp, &_jsi_TOP, (char*)typ);
                break;
            }
            case OP_INSTANCEOF: {

                jsiVarDeref(interp,2);
                int bval = Jsi_ValueInstanceOf(interp, _jsi_TOQ, _jsi_TOP);
                jsiPop(interp,1);
                Jsi_ValueMakeBool(interp, &_jsi_TOP, bval);
                break;
            }
            case OP_JTRUE:
            case OP_JFALSE: 
            case OP_JTRUE_NP:
            case OP_JFALSE_NP: {
                jsiVarDeref(interp,1);
                int off = (uintptr_t)ip->data - 1; 
                int r = Jsi_ValueIsTrue(interp, _jsi_TOP);
                
                if (ip->op == OP_JTRUE || ip->op == OP_JFALSE) jsiPop(interp,1);
                ip += ((ip->op == OP_JTRUE || ip->op == OP_JTRUE_NP) ^ r) ? 0 : off;
                break;
            }
            case OP_JMPPOP: 
                jsiPop(interp, ((jsi_JmpPopInfo *)ip->data)->topop);
            case OP_JMP: {
                int off = (ip->op == OP_JMP ? (uintptr_t)ip->data - 1
                            : (uintptr_t)((jsi_JmpPopInfo *)ip->data)->off - 1);

                while (1) {
                    if (trylist == NULL) break;
                    jsi_OpCode *tojmp = ip + off;

                    /* jmp out of a try block, should execute the finally block */
                    /* while jmp out a 'with' block, restore the scope */

                    if (trylist->type == jsi_TL_TRY) { 
                        if (tojmp >= trylist->d.td.tstart && tojmp < trylist->d.td.fend) break;
                        
                        if (ip >= trylist->d.td.tstart && ip < trylist->d.td.cend) {
                            trylist->d.td.last_op = jsi_LOP_JMP;
                            trylist->d.td.ld.tojmp = tojmp;
                            
                            ip = trylist->d.td.fstart - 1;
                            off = 0;
                            break;
                        } else if (ip >= trylist->d.td.fstart && ip < trylist->d.td.fend) {
                            pop_try(trylist);
                        } else Jsi_LogBug("jmp within a try, but not in its scope?");
                    } else {
                        /* with block */
                        
                        if (tojmp >= trylist->d.wd.wstart && tojmp < trylist->d.wd.wend) break;
                        
                        JSI_RESTORE_SCOPE();
                        pop_try(trylist);
                    }
                }
                
                ip += off;
                break;
            }
            case OP_EVAL: {
                int stackargc = (uintptr_t)ip->data;
                jsiVarDeref(interp, stackargc);

                int r = 0;
                Jsi_Value *spPtr = Jsi_ValueNew1(interp);
                if (stackargc > 0) {
                    if (_jsi_STACKIDX(interp->framePtr->Sp - stackargc)->vt == JSI_VT_UNDEF) {
                        Jsi_LogError("undefined value to eval()");
                        goto undef_eval;
                    }
                    int plen;
                    char *pro = Jsi_ValueString(interp, _jsi_STACKIDX(interp->framePtr->Sp - stackargc), &plen);
                    if (pro) {
                        pro = Jsi_StrdupLen(pro, plen);
                        r = jsiEvalOp(interp, ps, pro, scope, currentScope, _this, &spPtr);
                        Jsi_Free(pro);
                    } else {
                        Jsi_ValueCopy(interp, spPtr, _jsi_STACKIDX(interp->framePtr->Sp - stackargc));
                    }
                }
undef_eval:
                jsiPop(interp, stackargc);
                Jsi_ValueCopy(interp, _jsi_STACK[interp->framePtr->Sp], spPtr); /*TODO: is this correct?*/
                Jsi_DecrRefCount(interp, spPtr);
                jsiPush(interp,1);

                if (r) {
                    JSI_DO_THROW("eval");
                }
                break;
            }
            case OP_RET: {
                if (interp->framePtr->Sp>=1 && ip->data) {
                    jsiVarDeref(interp,1);
                    Jsi_ValueMove(interp, vret, _jsi_TOP);
                }
                jsiPop(interp, (uintptr_t)ip->data);
                interp->didReturn = 1;
                if (trylist) {
                    while (trylist) {
                        if (trylist->type == jsi_TL_TRY && trylist->inCatch)
                            JSI_RESTORE_SCOPE();
                        pop_try(trylist);
                    }
                    goto done;
                }
                ip = end;
                break;
            }
            case OP_DELETE: {
                int count = (uintptr_t)ip->data;
                if (count == 1) { // Non-standard.
                    if (_jsi_TOP->vt != JSI_VT_VARIABLE)
                        rc = Jsi_LogError("delete a right value");
                    else {
                        Jsi_Value **vPtr = &_jsi_TOP->d.lval, *v = *vPtr;
                        SIGASSERT(v, VALUE);
                        if (v->f.bits.dontdel) {
                            if (strict) rc = Jsi_LogWarn("delete not allowed");
                        } else if (v != currentScope) {
                            Jsi_ValueReset(interp,vPtr);     /* not allow to delete arguments */
                        }
                        else if (strict)
                            Jsi_LogWarn("Delete arguments");
                    }
                    jsiPop(interp,1);
                } else if (count == 2) {
                    jsiVarDeref(interp,2);
                    assert(interp->framePtr->Sp>=2);
                    if (strict) {
                        if (_jsi_TOQ->vt != JSI_VT_OBJECT) Jsi_LogWarn("delete non-object key, ignore");
                        if (_jsi_TOQ->d.obj == currentScope->d.obj) Jsi_LogWarn("Delete arguments");
                    }
                    jsi_ValueObjDelete(interp, _jsi_TOQ, _jsi_TOP, 0);
                    
                    jsiPop(interp,2);
                } else Jsi_LogBug("delete");
                break;
            }
            case OP_OBJECT: {
                int itemcount = (uintptr_t)ip->data;
                Assert(itemcount>=0);
                jsiVarDeref(interp, itemcount * 2);
                Jsi_Obj *obj = Jsi_ObjNewObj(interp, _jsi_STACK+(interp->framePtr->Sp-itemcount*2), itemcount*2);
                jsiPop(interp, itemcount * 2 - 1);       /* one left */
                jsiClearStack(interp,1);
                Jsi_ValueMakeObject(interp, &_jsi_TOP, obj);
                break;
            }
            case OP_ARRAY: {
                int itemcount = (uintptr_t)ip->data;
                Assert(itemcount>=0);
                jsiVarDeref(interp, itemcount);
                Jsi_Obj *obj = Jsi_ObjNewArray(interp, _jsi_STACK+(interp->framePtr->Sp-itemcount), itemcount, 1);
                jsiPop(interp, itemcount - 1);
                jsiClearStack(interp,1);
                Jsi_ValueMakeObject(interp, &_jsi_TOP, obj);
                break;
            }
            case OP_STRY: {
                jsi_TryInfo *ti = (jsi_TryInfo *)ip->data;
                jsi_TryList *n = jsi_trylist_new(jsi_TL_TRY, scope, currentScope);
                
                n->d.td.tstart = ip;                            /* make every thing pointed to right pos */
                n->d.td.tend = n->d.td.tstart + ti->trylen;
                n->d.td.cstart = n->d.td.tend + 1;
                n->d.td.cend = n->d.td.tend + ti->catchlen;
                n->d.td.fstart = n->d.td.cend + 1;
                n->d.td.fend = n->d.td.cend + ti->finallen;
                n->d.td.tsp = interp->framePtr->Sp;
                n->inCatch=0;
                n->inFinal=0;

                jsi_push_try(interp, &trylist, n);
                break;
            }
            case OP_ETRY: {             /* means nothing happen go to final */
                if (trylist == NULL || trylist->type != jsi_TL_TRY)
                    Jsi_LogBug("Unexpected ETRY opcode??");

                ip = trylist->d.td.fstart - 1;
                break;
            }
            case OP_SCATCH: {
                if (trylist == NULL || trylist->type != jsi_TL_TRY) 
                    Jsi_LogBug("Unexpected SCATCH opcode??");

                if (!ip->data) {
                    JSI_DO_THROW("catch");
                } else {
                    trylist->inCatch=1;
                    /* new scope and make var */
                    scope = jsi_ScopeChainDupNext(interp, scope, currentScope);
                    currentScope = jsi_ObjValueNew(interp);
                    interp->framePtr->ingsc = scope;  //TODO: changing frame
                    interp->framePtr->incsc = currentScope;
                    Jsi_IncrRefCount(interp, currentScope);
                    Jsi_Value *excpt = Jsi_ValueNew1(interp);
                    if (ps->last_exception && ps->last_exception->vt != JSI_VT_UNDEF) {
                        //TODO: fix test262 crash in freeValueTbl@jsiInterp.c:565 for last_exception which is
                        // freed in jsi_PstateFree@jsiPstate.c:251. Is this code the problem?
                        Jsi_Value *ple = ps->last_exception;
                        Jsi_ValueCopy(interp, excpt, ple);
                        Jsi_ValueReset(interp, &ps->last_exception);
                    } else if (interp->errMsgBuf[0]) {
                        Jsi_ValueMakeStringDup(interp, &excpt, interp->errMsgBuf);
                        interp->errMsgBuf[0] = 0;
                    }
                    Jsi_ValueInsert(interp, currentScope, (char*)ip->data, excpt, JSI_OM_DONTENUM);
                    Jsi_DecrRefCount(interp, excpt);
                    context_id = ps->_context_id++;
                }
                break;
            }
            case OP_ECATCH: {
                if (trylist == NULL || trylist->type != jsi_TL_TRY)
                    Jsi_LogBug("Unexpected ECATCH opcode??");

                trylist->inCatch=0;
                ip = trylist->d.td.fstart - 1;
                break;
            }
            case OP_SFINAL: {
                if (trylist == NULL || trylist->type != jsi_TL_TRY)
                    Jsi_LogBug("Unexpected SFINAL opcode??");

                /* restore scatch scope chain */
                trylist->inFinal = 1;
                JSI_RESTORE_SCOPE();
                break;
            }
            case OP_EFINAL: {
                if (trylist == NULL || trylist->type != jsi_TL_TRY)
                    Jsi_LogBug("Unexpected EFINAL opcode??");

                trylist->inFinal = 0;
                int last_op = trylist->d.td.last_op;
                jsi_OpCode *tojmp = (last_op == jsi_LOP_JMP ? trylist->d.td.ld.tojmp : 0);
                
                pop_try(trylist);

                if (last_op == jsi_LOP_THROW) {
                    JSI_DO_THROW("finally");
                } else if (last_op == jsi_LOP_JMP) {
                    while (1) {
                        if (trylist == NULL) {
                            ip = tojmp;
                            break;
                        }
                        /* same as jmp opcode, see above */
                        if (trylist->type == jsi_TL_TRY) {
                            if (tojmp >= trylist->d.td.tstart && tojmp < trylist->d.td.fend) {
                                ip = tojmp;
                                break;
                            }
                            
                            if (ip >= trylist->d.td.tstart && ip < trylist->d.td.cend) {
                                trylist->d.td.last_op = jsi_LOP_JMP;
                                trylist->d.td.ld.tojmp = tojmp;
                                
                                ip = trylist->d.td.fstart - 1;
                                break;
                            } else if (ip >= trylist->d.td.fstart && ip < trylist->d.td.fend) {
                                pop_try(trylist);
                            } else Jsi_LogBug("jmp within a try, but not in its scope?");
                        } else {        /* 'with' block */
                            if (tojmp >= trylist->d.wd.wstart && tojmp < trylist->d.wd.wend) {
                                ip = tojmp;
                                break;
                            }
                            JSI_RESTORE_SCOPE();
                            pop_try(trylist);
                        }
                    }
                }
                break;
            }
            case OP_THROW: {
                jsiVarDeref(interp,1);
                Jsi_ValueDup2(interp,&ps->last_exception, _jsi_TOP);
                interp->didReturn = 1; /* TODO: could possibly hide _jsi_STACK problem */
                JSI_DO_THROW("throw");
                break;
            }
            case OP_WITH: {
                static int warnwith = 1;
                if (strict && warnwith && interp->typeCheck.nowith) {
                    warnwith = 0;
                    rc = Jsi_LogError("use of with is illegal due to \"use nowith\"");
                    break;
                }
                jsiVarDeref(interp,1);
                Jsi_ValueToObject(interp, _jsi_TOP);
                
                jsi_TryList *n = jsi_trylist_new(jsi_TL_WITH, scope, currentScope);
                
                n->d.wd.wstart = ip;
                n->d.wd.wend = n->d.wd.wstart + (uintptr_t)ip->data;

                jsi_push_try(interp, &trylist, n);
                interp->framePtr->withDepth++;
                
                /* make expr to top of scope chain */
                scope = jsi_ScopeChainDupNext(interp, scope, currentScope);
                currentScope = Jsi_ValueNew1(interp);
                interp->framePtr->ingsc = scope;
                interp->framePtr->incsc = currentScope;
                Jsi_ValueCopy(interp, currentScope, _jsi_TOP);
                jsiPop(interp,1);
                
                context_id = ps->_context_id++;
                break;
            }
            case OP_EWITH: {
                if (trylist == NULL || trylist->type != jsi_TL_WITH)
                    Jsi_LogBug("Unexpected EWITH opcode??");

                JSI_RESTORE_SCOPE();
                
                pop_try(trylist);
                interp->framePtr->withDepth--;
                break;
            }
            case OP_DEBUG: {
                jsi_DebuggerStmt();
                jsiPush(interp,1);
                break;
            }
            case OP_RESERVED: {
                jsi_ReservedInfo *ri = (jsi_ReservedInfo *)ip->data;
                const char *cmd = ri->type == RES_CONTINUE ? "continue" : "break";
                /* TODO: continue/break out of labeled scope: see tests/prob/break.jsi. */
                if (ri->label) {
                    Jsi_LogError("%s: label(%s) not found", cmd, ri->label);
                } else {
                    Jsi_LogError("%s must be inside loop(or switch)", cmd);
                }
                rc = JSI_ERROR;
                break;
            }
#ifndef __cplusplus
            default:
                Jsi_LogBug("invalid op ceod: %d", ip->op);
#endif
        }
        lop = plop;
        ip++;
    }
done:
    while (trylist) {
        JSI_RESTORE_SCOPE();
        pop_try(trylist);
    }
    return rc;
}