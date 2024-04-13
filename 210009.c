void jsi_ValueDebugLabel_(jsi_ValueDebug *vd, const char *l1, const char *l2)
{
    if (l1)
        vd->label = l1;
    if (l2)
        vd->label2 = l2;
}