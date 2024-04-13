nft_fwd_select_ops(const struct nft_ctx *ctx,
		   const struct nlattr * const tb[])
{
	if (tb[NFTA_FWD_SREG_ADDR])
		return &nft_fwd_neigh_netdev_ops;
	if (tb[NFTA_FWD_SREG_DEV])
		return &nft_fwd_netdev_ops;

        return ERR_PTR(-EOPNOTSUPP);
}