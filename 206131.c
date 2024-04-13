mysql_ssl_set(MYSQL *mysql __attribute__((unused)) ,
	      const char *key __attribute__((unused)),
	      const char *cert __attribute__((unused)),
	      const char *ca __attribute__((unused)),
	      const char *capath __attribute__((unused)),
	      const char *cipher __attribute__((unused)))
{
  DBUG_ENTER("mysql_ssl_set");
#if defined(HAVE_OPENSSL) && !defined(EMBEDDED_LIBRARY)
  my_free(mysql->options.ssl_key);
  my_free(mysql->options.ssl_cert);
  my_free(mysql->options.ssl_ca);
  my_free(mysql->options.ssl_capath);
  my_free(mysql->options.ssl_cipher);
  mysql->options.ssl_key=    set_ssl_option_unpack_path(key);
  mysql->options.ssl_cert=   set_ssl_option_unpack_path(cert);
  mysql->options.ssl_ca=     set_ssl_option_unpack_path(ca);
  mysql->options.ssl_capath= set_ssl_option_unpack_path(capath);
  mysql->options.ssl_cipher= strdup_if_not_null(cipher);
#endif /* HAVE_OPENSSL && !EMBEDDED_LIBRARY */
  mysql->options.use_ssl= TRUE;
  DBUG_RETURN(0);
}