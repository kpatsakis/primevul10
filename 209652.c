const char *jsi_ScopeStrsGet(Jsi_ScopeStrs *ss, int i)
{
    if (i < 0 || i >= ss->count)
        return NULL;
    return ss->args[i].name;
}