static int detect_signtrust(
	sc_pkcs15_card_t *p15card
){
	if(insert_cert(p15card,"8000DF01C000", 0x45, 1, "Signatur Zertifikat")) return 1;
	set_string(&p15card->tokeninfo->manufacturer_id, "Deutsche Post");
	set_string(&p15card->tokeninfo->label, "SignTrust Card");

	insert_cert(p15card,"800082008220", 0x46, 1, "Verschluesselungs Zertifikat");
	insert_cert(p15card,"800083008320", 0x47, 1, "Authentifizierungs Zertifikat");

	insert_key(p15card,"8000DF015331", 0x45, 0x80, 1024, 1, "Signatur Schluessel");
	insert_key(p15card,"800082008210", 0x46, 0x80, 1024, 2, "Verschluesselungs Schluessel");
	insert_key(p15card,"800083008310", 0x47, 0x80, 1024, 3, "Authentifizierungs Schluessel");

	insert_pin(p15card,"8000DF010000", 1, 0, 0x81, 6, "Signatur PIN",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
		SC_PKCS15_PIN_FLAG_INITIALIZED
	);
	insert_pin(p15card,"800082000040", 2, 0, 0x81, 6, "Verschluesselungs PIN",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
		SC_PKCS15_PIN_FLAG_INITIALIZED
	);
	insert_pin(p15card,"800083000040", 3, 0, 0x81, 6, "Authentifizierungs PIN",
		SC_PKCS15_PIN_FLAG_CASE_SENSITIVE | SC_PKCS15_PIN_FLAG_LOCAL |
		SC_PKCS15_PIN_FLAG_INITIALIZED
	);

	return 0;
}