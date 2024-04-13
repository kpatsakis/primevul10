static bool interpObjIsTrue(void *data)
{
    InterpObj *fo = (InterpObj *)data;
    SIGASSERT(fo,INTERPOBJ);
    if (!fo->subinterp) return 0;
    else return 1;
}