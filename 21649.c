unbind_args ()
{
  remember_args ((WORD_LIST *)NULL, 1);
  pop_args ();				/* Reset BASH_ARGV and BASH_ARGC */
}