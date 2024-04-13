const char *jsi_GetCurFile(Jsi_Interp *interp)
{
    const char *curFile = NULL;
    if (!interp)
        return NULL;
    if (interp->inParse)
        curFile = interp->curFile;
    else
        curFile = (interp->curIp && interp->curIp->fname? interp->curIp->fname:interp->curFile);
    if (!curFile) curFile = interp->framePtr->fileName;
    if (!curFile) curFile = "";
    return curFile;
}