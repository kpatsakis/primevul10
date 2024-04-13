void Jsi_NumberItoA10(Jsi_Wide value, char* buf, int bsiz)
{
    snprintf(buf, bsiz, "%" PRId64, value);
}