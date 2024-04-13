run_pcl(struct pcilynx *lynx, dma_addr_t pcl_bus,
			   int dmachan)
{
	reg_write(lynx, DMA0_CURRENT_PCL + dmachan * 0x20, pcl_bus);
	reg_write(lynx, DMA0_CHAN_CTRL + dmachan * 0x20,
		  DMA_CHAN_CTRL_ENABLE | DMA_CHAN_CTRL_LINK);
}