bool Jsi_ValueIsFunction(Jsi_Interp *interp, Jsi_Value *v)
{
    int rc = (v!=NULL && v->vt == JSI_VT_OBJECT && v->d.obj->ot == JSI_OT_FUNCTION);
    if (!rc) return rc;
    if (interp == v->d.obj->d.fobj->interp)
        return 1;
    fprintf(stderr, "OOPS: function in wrong interp %s: %s\n", 
        interp->parent?"(string came in from parent interp?)":"",
        v->d.obj->d.fobj->func->name);
    return 0;
}