close_pam_conf(struct pam_fh *pam_fh)
{
	(void) munmap(pam_fh->data, pam_fh->bufsize);
	(void) close(pam_fh->fconfig);
	free(pam_fh);
}