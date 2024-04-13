sv_lower(pTHX_ SV* sv)
{
    STRLEN len;
    char *s = SvPV_force(sv, len);
    for (; len--; s++)
	*s = toLOWER(*s);
    return sv;
}