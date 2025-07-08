#pragma once

#include <maya/MViewport2Renderer.h>
#include "RenderTargetsInterface.h"
#include "CustomRenderProcess.h"

class OverridePresentTarget : public MHWRender::MPresentTarget, public RenderTargetsInterface, public amagly::CustomRenderProcess
{
public:
	OverridePresentTarget(const MString& name);
	~OverridePresentTarget() override;

	MStatus preProcess() override;
	MStatus postProcess() override;
	MHWRender::MRenderOperation *renderOperation() override
	{
		return (MHWRender::MRenderOperation*)this;
	}

	MHWRender::MRenderTarget* const* targetOverrideList(unsigned int& listSize) override;
};

