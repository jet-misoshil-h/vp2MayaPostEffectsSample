#include "RenderTargetsInterface.h"

RenderTargetsInterface::RenderTargetsInterface()
: mTargets(nullptr)
{
}

RenderTargetsInterface::~RenderTargetsInterface()
{
}

void RenderTargetsInterface::setRenderTargets(MHWRender::MRenderTarget** targets)
{
	mTargets = targets;
}

MHWRender::MRenderTarget* const* RenderTargetsInterface::getRenderTargets(unsigned int& listSize)
{
	if (mTargets)
	{
		listSize = mOutputTargetCount;
		return &mTargets[0];
	}
	listSize = 0;
	return nullptr;
}

