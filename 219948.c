u_save_line(linenr_T lnum)
{
    return vim_strsave(ml_get(lnum));
}