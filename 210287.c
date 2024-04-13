void Decoder::getOrCreateCallTarget(
		utils::Address addr,
		llvm::Function*& tFnc,
		llvm::BasicBlock*& tBb)
{
	tBb = nullptr;
	tFnc = nullptr;

	if (auto* f = getFunctionAtAddress(addr))
	{
		tFnc = f;
		tBb = tFnc->empty() ? nullptr : &tFnc->front();
		LOG << "\t\t\t\t" << "F: getFunctionAtAddress() @ " << addr << std::endl;
	}
	else if (auto* f = splitFunctionOn(addr))
	{
		tFnc = f;
		tBb = tFnc->empty() ? nullptr : &tFnc->front();
		LOG << "\t\t\t\t" << "F: splitFunctionOn() @ " << addr << std::endl;
	}
	else if (auto* bb = getBasicBlockAtAddress(addr))
	{
		tBb = bb;
		LOG << "\t\t\t\t" << "F: getBasicBlockAtAddress() @ " << addr << std::endl;
	}
	else if (getBasicBlockContainingAddress(addr))
	{
		// Nothing - we are not splitting BBs here.
		LOG << "\t\t\t\t" << "F: getBasicBlockContainingAddress() @ "
				<< addr << std::endl;
	}
	else if (getFunctionContainingAddress(addr))
	{
		auto* bb = getBasicBlockBeforeAddress(addr);
		assert(bb);
		tBb = createBasicBlock(addr, bb->getParent(), bb);
		LOG << "\t\t\t\t" << "F: getFunctionContainingAddress() @ "
				<< addr << std::endl;
	}
	else
	{
		tFnc = createFunction(addr);
		tBb = tFnc && !tFnc->empty() ? &tFnc->front() : nullptr;
		LOG << "\t\t\t\t" << "F: createFunction() @ "
				<< addr << std::endl;
	}
}