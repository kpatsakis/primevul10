void Jsi_NumberDtoA(Jsi_Interp *interp, Jsi_Number value, char* buf, int bsiz, int prec)
{
    int dp = interp->subOpts.dblPrec-1, dm = __DBL_DECIMAL_DIG__;
    if (prec==0)
        prec = (dp<=0?dm+dp:dp);
    else if (prec<0)
            prec = dm+prec;
    if (prec<=0)
        prec = dm-1;
    if (Jsi_NumberIsNaN(value))
        Jsi_Strcpy(buf,"NaN");
    else
        snprintf(buf, bsiz, "%.*" JSI_NUMGFMT, prec, value);
}