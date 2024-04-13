void jsi_ValueDebugUpdate_(Jsi_Interp *interp, jsi_ValueDebug *vd, void *v, Jsi_Hash* tbl, const char *fname, int line, const char *func)
{
    vd->fname = fname;
    vd->line = line;
    vd->func = func;
    if (!vd->Idx)
        vd->Idx = interp->dbPtr->memDebugCallIdx;
    vd->hPtr = Jsi_HashSet(tbl, v, 0);
    vd->ip = interp->curIp;
    if (vd->ip) {
        vd->ipLine = vd->ip->Line;
        vd->ipOp = vd->ip->op;
        vd->ipFname = vd->ip->fname;
    }
    vd->interp = interp;
    if (jsi_memDebugBreakIdx && jsi_memDebugBreakIdx == vd->Idx)
        jsi_memDebugBreak();
}