void selinux_netlbl_err(struct sk_buff *skb, int error, int gateway)
{
	netlbl_skbuff_err(skb, error, gateway);
}