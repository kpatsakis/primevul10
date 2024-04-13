void cgit_clone_head(void)
{
	send_file(git_path("%s", "HEAD"));
}