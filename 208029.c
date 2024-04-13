free_global ()
{
     xfree(clamd_local);
     xfree(clamd_ip);
     xfree(clamd_port);
     xfree(clamd_curr_ip);
     xfree(redirect_url);
     if (patterns != NULL) {
	while (pattc > 0) {
	   pattc--;
	   regfree(&patterns[pattc].regexv);
	   xfree(patterns[pattc].pattern);
	}
	free(patterns);
	patterns = NULL;
     }
}