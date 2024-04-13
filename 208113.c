free_env(env_list *pam_env)
{
	if (pam_env) {
		if (pam_env->name)
			free(pam_env->name);
		if (pam_env->value)
			free(pam_env->value);
		free(pam_env);
	}
}