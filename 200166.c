issue_expect_message (int expected)
{
  if (expected == '\n')
    M4ERROR ((EXIT_FAILURE, 0, "expecting line feed in frozen file"));
  else
    M4ERROR ((EXIT_FAILURE, 0, "expecting character `%c' in frozen file",
	      expected));
}