sc_oberthur_get_friends (unsigned int id, struct crypto_container *ccont)
{
	struct container *cont;

	for (cont = Containers; cont; cont = cont->next)   {
		if (cont->exchange.id_pub == id || cont->exchange.id_prv == id || cont->exchange.id_cert == id)   {
			if (ccont)
				memcpy(ccont, &cont->exchange, sizeof(struct crypto_container));
			break;
		}

		if (cont->sign.id_pub == id || cont->sign.id_prv == id || cont->sign.id_cert == id)   {
			if (ccont)
				memcpy(ccont, &cont->sign, sizeof(struct crypto_container));
			break;
		}
	}

	return cont ? 0 : SC_ERROR_TEMPLATE_NOT_FOUND;
}