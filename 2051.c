void Socket::stopSsl()
{
#ifdef NETDEBUG
    std::cout << thread_id << "ssl stopping" << std::endl;
#endif
    if(!isssl) return;

    isssl = false;

    if (ssl != NULL) {
        if (issslserver) {
#ifdef NETDEBUG
            std::cout << thread_id << "this is a server connection" << std::endl;
            if (SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN) {
                std::cout << thread_id << "SSL_SENT_SHUTDOWN IS SET" << std::endl;
            }
            if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
                std::cout << thread_id << "SSL_RECEIVED_SHUTDOWN IS SET" << std::endl;
            }
            std::cout << thread_id << "calling 1st ssl shutdown" << std::endl;
#endif
            if (!SSL_shutdown(ssl)) {
#ifdef NETDEBUG
                std::cout << thread_id << "need to call SSL shutdown again" << std::endl;
                if (SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN) {
                    std::cout << thread_id << "SSL_SENT_SHUTDOWN IS SET" << std::endl;
                }
                if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
                    std::cout << thread_id << "SSL_RECEIVED_SHUTDOWN IS SET" << std::endl;
                }
                std::cout << thread_id << "Discarding extra data from client" << std::endl;
#endif

                shutdown(SSL_get_fd(ssl), SHUT_WR);
                char junk[1024];
                readFromSocket(junk, sizeof(junk), 0, 5);
#ifdef NETDEBUG
                std::cout << thread_id << "done" << std::endl;
#endif
            }
        } else {
#ifdef NETDEBUG
            std::cout << thread_id << "this is a client connection" << std::endl;
            if (SSL_get_shutdown(ssl) & SSL_SENT_SHUTDOWN) {
                std::cout << thread_id << "SSL_SENT_SHUTDOWN IS SET" << std::endl;
            }
            if (SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN) {
                std::cout << thread_id << "SSL_RECEIVED_SHUTDOWN IS SET" << std::endl;
            }
            std::cout << thread_id << "calling ssl shutdown" << std::endl;
#endif
            SSL_shutdown(ssl);
#ifdef NETDEBUG
            std::cout << thread_id << "done" << std::endl;
#endif
        }
    }

    cleanSsl();

}