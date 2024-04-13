int ssl_connect(int sock, SSL **ssl_con) {
   SSL_METHOD *meth;
   SSL_CTX *ctx;
   X509 *cert = NULL;
   int i;

   SSL_library_init();
   SSL_load_error_strings();

   meth = (SSL_METHOD *) TLSv1_2_method();
   ctx = SSL_CTX_new(meth);

   *ssl_con = SSL_new(ctx);
   SSL_set_fd(*ssl_con, sock);
   if (SSL_connect(*ssl_con) <= 0)
      return -1;

   cert = SSL_get_peer_certificate(*ssl_con);
   if (cert == NULL)
      return -1;

   i = SSL_get_verify_result(*ssl_con);
   if (i != X509_V_OK)
      printf("Possibly invalid certificate, continue on your own risk!\n");

   return 0;
}