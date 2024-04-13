llvm::GlobalVariable* Decoder::getCallReturnObject()
{
	if (_config->getConfig().architecture.isX86_32())
	{
		return _abi->getRegister(X86_REG_EAX);
	}
	else if (_config->getConfig().architecture.isX86_64())
	{
		return _abi->getRegister(X86_REG_RAX);
	}
	else if (_config->getConfig().architecture.isMipsOrPic32())
	{
		return _abi->getRegister(MIPS_REG_V0);
	}
	else if (_config->getConfig().architecture.isPpc())
	{
		return _abi->getRegister(PPC_REG_R3);
	}
	else if (_config->getConfig().architecture.isArm32OrThumb())
	{
		return _abi->getRegister(ARM_REG_R0);
	}
	else if (_config->getConfig().architecture.isArm64())
	{
		return _config->getLlvmRegister("r0");
	}

	assert(false);
	return nullptr;
}