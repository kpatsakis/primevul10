add_shopt_to_alist (opt, on_or_off)
     char *opt;
     int on_or_off;
{
  if (shopt_ind >= shopt_len)
    {
      shopt_len += 8;
      shopt_alist = (STRING_INT_ALIST *)xrealloc (shopt_alist, shopt_len * sizeof (shopt_alist[0]));
    }
  shopt_alist[shopt_ind].word = opt;
  shopt_alist[shopt_ind].token = on_or_off;
  shopt_ind++;
}