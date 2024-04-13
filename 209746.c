char* Jsi_NumberToString(Jsi_Interp *interp, Jsi_Number d, char *buf, int bsiz)
{
     if (Jsi_NumberIsInteger(d)) {
        Jsi_NumberItoA10((Jsi_Wide)d, buf, bsiz);
    } else if (Jsi_NumberIsNormal(d)) {
        Jsi_NumberDtoA(interp, d, buf, bsiz, 0);
    } else if (Jsi_NumberIsNaN(d)) {
        Jsi_Strcpy(buf, "NaN");
    } else {
        int s = Jsi_NumberIsInfinity(d);
        if (s > 0) Jsi_Strcpy(buf,  "Infinity");
        else if (s < 0) Jsi_Strcpy(buf, "-Infinity");
        else {
            buf[0] = 0;
        }
    }
    return buf;
}