int Socket::checkCertHostname(const std::string &_hostname)
{
#if OPENSSL_VERSION_NUMBER < 0x10100000L
    String hostname = _hostname;

    X509 *peercertificate = SSL_get_peer_certificate(ssl);
    if (peercertificate == NULL) {
#ifdef NETDEBUG
        std::cout << thread_id << "unable to get certificate for " << hostname << std::endl;
#endif
        return -1;
    }
    //force to lower case as domain names are not case sensetive
    hostname.toLower();

#ifdef NETDEBUG
    std::cout << thread_id << "checking certificate" << hostname << std::endl;
    std::cout << thread_id << "Checking hostname against subjectAltNames" << std::endl;
#endif


    bool matched = false;
    bool hasaltname = false;

    //check the altname extension for additional valid names
    STACK_OF(GENERAL_NAME) *gens = NULL;
    gens = (STACK_OF(GENERAL_NAME) *)X509_get_ext_d2i(peercertificate, NID_subject_alt_name, 0, 0);
    int r = sk_GENERAL_NAME_num(gens);
    for (int i = 0; i < r; ++i) {
        const GENERAL_NAME *gn = sk_GENERAL_NAME_value(gens, i);

        //if its not a dns entry we really dont care about it
        if (gn->type != GEN_DNS) {
            continue;
        }

        //only mark hasaltname as true if it has a DNS altname
        hasaltname = true;

        //an ASN1_IA5STRING is a define of an ASN1_STRING so we can do it this way
        unsigned char *nameutf8;
        int len = ASN1_STRING_to_UTF8(&nameutf8, gn->d.ia5);
        if (len < 0) {
            break;
        }

        String altname = std::string((char *)nameutf8, len);
        OPENSSL_free(nameutf8);

        //force to lower case as domain names are not case sensetive
        altname.toLower();

#ifdef NETDEBUG
        std::cout << thread_id << "checking against alt name " << altname << std::endl;
#endif

        if (hostname.compare(altname) == 0) {
            matched = true;
            break;
        } else if (altname.contains("*")) {
#ifdef NETDEBUG
            std::cout << thread_id << "Wildcard certificate is in use" << std::endl;
#endif
            String  anend;
            anend = altname.after("*"); // need to keep the "."
            if (hostname.endsWith(anend)) {
                bool part_match = true;
                String anstart = altname.before("*");
                String t = hostname.before(anend.c_str());
                if( anstart.length() > 0) {             // if something before * we must also match this
                  if( hostname.startsWith(anstart)) {
                    t = t.after(anstart.c_str());
                  } else {
                      part_match = false;    // even though after * matches, no match on before * - so cannot match
                   }
                 }
                 //    t now contains what is matched by the '*"  - this must not contain a '.'
                 if (part_match && !t.contains(".")) {
                   matched = true;
                   break;
                }
            }
        }
    }
    sk_GENERAL_NAME_pop_free(gens, GENERAL_NAME_free);

    if (matched) {
        X509_free(peercertificate);
        return 0;
    } else if (hasaltname) {
        X509_free(peercertificate);
        return -1;
    }

#ifdef NETDEBUG
    std::cout << thread_id << "checking hostname against the following common names" << std::endl;
#endif

    X509_NAME *name = X509_get_subject_name(peercertificate);

    int current_entry = -1;
    while (1) {

        //get the common name from the certificate
        current_entry = X509_NAME_get_index_by_NID(name, NID_commonName, current_entry);
        if (current_entry == -1) {
            //if we've run out of common names then move on to altnames
            break;
        }

        //X509_NAME_get_entry result must not be freed
        X509_NAME_ENTRY *entry = X509_NAME_get_entry(name, current_entry);

        ASN1_STRING *asn1name = X509_NAME_ENTRY_get_data(entry);

        unsigned char *nameutf8;
        int len = ASN1_STRING_to_UTF8(&nameutf8, asn1name);
        if (len < 0) {
            break;
        }
        String commonname = std::string((char *)nameutf8, len);

        OPENSSL_free(nameutf8);

        //force to lower case as domain names are not case sensetive
        commonname.toLower();

#ifdef NETDEBUG
        std::cout << thread_id << "checking against common name " << commonname << std::endl;
#endif

        //compare the hostname to the common name
        if (hostname.compare(commonname) == 0) {
            matched = true;
            break;
        }
        //see if its a wildcard certificate
        else if (commonname.startsWith("*.")) {
#ifdef NETDEBUG
            std::cout << thread_id << "Wildcard certificate is in use" << std::endl;
#endif
            commonname = commonname.after("*"); // need to keep the "."

            if (hostname.endsWith(commonname)) {
                matched = true;
                break;
            }
        }
    }

    if (matched) {
        X509_free(peercertificate);
        return 0;
    }
#else  // is openssl v1.1 or above
    return 0;    //TODO
#endif
    return -1;
}