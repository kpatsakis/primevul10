void Jsi_NumberUtoA10(Jsi_UWide value, char* buf, int bsiz)
{
    snprintf(buf, bsiz, "%" PRIu64, value);
}