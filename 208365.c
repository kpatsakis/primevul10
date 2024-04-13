PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh,int flags,int argc,CONST char **argv)
{
	int retval, *pret;

	retval = PAM_SUCCESS;
	pret = &retval;
	pam_get_data(pamh, "rad_setcred_return", (CONST void **) &pret);
	return *pret;
}