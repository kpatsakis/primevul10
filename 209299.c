static Jsi_RC KeyLocker(Jsi_Hash* tbl, int lock)
{
    if (!lock)
        Jsi_MutexUnlock(jsiIntData.mainInterp, jsiIntData.mainInterp->Mutex);
    else
        return Jsi_MutexLock(jsiIntData.mainInterp, jsiIntData.mainInterp->Mutex);
    return JSI_OK;
}