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
  mysql->options.ssl_key=    strdup_if_not_null(key);
  mysql->options.ssl_cert=   strdup_if_not_null(cert);
  mysql->options.ssl_ca=     strdup_if_not_null(ca);
  mysql->options.ssl_capath= strdup_if_not_null(capath);
  mysql->options.ssl_cipher= strdup_if_not_null(cipher);
  mysql->options.use_ssl= TRUE;
#endif /* HAVE_OPENSSL && !EMBEDDED_LIBRARY */
  DBUG_RETURN(0);
}