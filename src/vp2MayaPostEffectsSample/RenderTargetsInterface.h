#pragma once

#include <maya/MViewport2Renderer.h>

class RenderTargetsInterface
{
public:
	RenderTargetsInterface();
	~RenderTargetsInterface();

	void setTargetCount(unsigned int count) { mOutputTargetCount = count; }
	virtual void setRenderTargets(MHWRender::MRenderTarget** targets);
	virtual MHWRender::MRenderTarget* const* getRenderTargets(unsigned int& listSize);

protected:
	MHWRender::MRenderTarget** mTargets;
	unsigned int mOutputTargetCount;
};