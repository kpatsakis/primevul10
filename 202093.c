static int __ipv6_regen_rndid(struct inet6_dev *idev)
{
	struct net_device *dev;
	struct scatterlist sg[2];

	sg[0].page = virt_to_page(idev->entropy);
	sg[0].offset = offset_in_page(idev->entropy);
	sg[0].length = 8;
	sg[1].page = virt_to_page(idev->work_eui64);
	sg[1].offset = offset_in_page(idev->work_eui64);
	sg[1].length = 8;

	dev = idev->dev;

	if (ipv6_generate_eui64(idev->work_eui64, dev)) {
		printk(KERN_INFO
			"__ipv6_regen_rndid(idev=%p): cannot get EUI64 identifier; use random bytes.\n",
			idev);
		get_random_bytes(idev->work_eui64, sizeof(idev->work_eui64));
	}
regen:
	spin_lock(&md5_tfm_lock);
	if (unlikely(md5_tfm == NULL)) {
		spin_unlock(&md5_tfm_lock);
		return -1;
	}
	crypto_digest_init(md5_tfm);
	crypto_digest_update(md5_tfm, sg, 2);
	crypto_digest_final(md5_tfm, idev->work_digest);
	spin_unlock(&md5_tfm_lock);

	memcpy(idev->rndid, &idev->work_digest[0], 8);
	idev->rndid[0] &= ~0x02;
	memcpy(idev->entropy, &idev->work_digest[8], 8);

	/*
	 * <draft-ietf-ipngwg-temp-addresses-v2-00.txt>:
	 * check if generated address is not inappropriate
	 *
	 *  - Reserved subnet anycast (RFC 2526)
	 *	11111101 11....11 1xxxxxxx
	 *  - ISATAP (draft-ietf-ngtrans-isatap-13.txt) 5.1
	 *	00-00-5E-FE-xx-xx-xx-xx
	 *  - value 0
	 *  - XXX: already assigned to an address on the device
	 */
	if (idev->rndid[0] == 0xfd && 
	    (idev->rndid[1]&idev->rndid[2]&idev->rndid[3]&idev->rndid[4]&idev->rndid[5]&idev->rndid[6]) == 0xff &&
	    (idev->rndid[7]&0x80))
		goto regen;
	if ((idev->rndid[0]|idev->rndid[1]) == 0) {
		if (idev->rndid[2] == 0x5e && idev->rndid[3] == 0xfe)
			goto regen;
		if ((idev->rndid[2]|idev->rndid[3]|idev->rndid[4]|idev->rndid[5]|idev->rndid[6]|idev->rndid[7]) == 0x00)
			goto regen;
	}

	return 0;
}