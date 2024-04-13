read_cc_bcc (compose_env_t *env)
{
  if (mailvar_is_true (mailvar_name_askcc))
    compose_header_set (env, MU_HEADER_CC,
			ml_readline_with_intr ("Cc: "), COMPOSE_REPLACE);
  if (mailvar_is_true (mailvar_name_askbcc))
    compose_header_set (env, MU_HEADER_BCC,
			ml_readline_with_intr ("Bcc: "), COMPOSE_REPLACE);
}