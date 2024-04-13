static pj_status_t decode_uint_attr(pj_pool_t *pool, 
				    const pj_uint8_t *buf, 
				    const pj_stun_msg_hdr *msghdr, 
				    void **p_attr)
{
    pj_stun_uint_attr *attr;

    PJ_UNUSED_ARG(msghdr);

    /* Create the attribute */
    attr = PJ_POOL_ZALLOC_T(pool, pj_stun_uint_attr);
    GETATTRHDR(buf, &attr->hdr);

    attr->value = GETVAL32H(buf, 4);

    /* Check that the attribute length is valid */
    if (attr->hdr.length != 4)
	return PJNATH_ESTUNINATTRLEN;

    /* Done */
    *p_attr = attr;

    return PJ_SUCCESS;
}