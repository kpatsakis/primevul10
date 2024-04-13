free_pipe ()
{
     xfree(squidguard);
     if (sgfpw) fclose(sgfpw);
     if (sgfpr) fclose(sgfpr);
}