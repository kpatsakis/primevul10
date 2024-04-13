int ipmr_ioctl(struct sock *sk, int cmd, void __user *arg)
{
	struct sioc_sg_req sr;
	struct sioc_vif_req vr;
	struct vif_device *vif;
	struct mfc_cache *c;
	
	switch(cmd)
	{
		case SIOCGETVIFCNT:
			if (copy_from_user(&vr,arg,sizeof(vr)))
				return -EFAULT; 
			if(vr.vifi>=maxvif)
				return -EINVAL;
			read_lock(&mrt_lock);
			vif=&vif_table[vr.vifi];
			if(VIF_EXISTS(vr.vifi))	{
				vr.icount=vif->pkt_in;
				vr.ocount=vif->pkt_out;
				vr.ibytes=vif->bytes_in;
				vr.obytes=vif->bytes_out;
				read_unlock(&mrt_lock);

				if (copy_to_user(arg,&vr,sizeof(vr)))
					return -EFAULT;
				return 0;
			}
			read_unlock(&mrt_lock);
			return -EADDRNOTAVAIL;
		case SIOCGETSGCNT:
			if (copy_from_user(&sr,arg,sizeof(sr)))
				return -EFAULT;

			read_lock(&mrt_lock);
			c = ipmr_cache_find(sr.src.s_addr, sr.grp.s_addr);
			if (c) {
				sr.pktcnt = c->mfc_un.res.pkt;
				sr.bytecnt = c->mfc_un.res.bytes;
				sr.wrong_if = c->mfc_un.res.wrong_if;
				read_unlock(&mrt_lock);

				if (copy_to_user(arg,&sr,sizeof(sr)))
					return -EFAULT;
				return 0;
			}
			read_unlock(&mrt_lock);
			return -EADDRNOTAVAIL;
		default:
			return -ENOIOCTLCMD;
	}
}