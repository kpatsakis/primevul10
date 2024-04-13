grow_gap(pTHX_ SV* sv, STRLEN grow, char** t, char** s, char** e)
{
    /*
     SvPVX ---> AAAAAA...BBBBBB
                     ^   ^     ^
                     t   s     e
    */
    STRLEN t_offset = *t - SvPVX(sv);
    STRLEN s_offset = *s - SvPVX(sv);
    STRLEN e_offset = *e - SvPVX(sv);

    SvGROW(sv, e_offset + grow + 1);

    *t = SvPVX(sv) + t_offset;
    *s = SvPVX(sv) + s_offset;
    *e = SvPVX(sv) + e_offset;

    Move(*s, *s+grow, *e - *s, char);
    *s += grow;
    *e += grow;
}