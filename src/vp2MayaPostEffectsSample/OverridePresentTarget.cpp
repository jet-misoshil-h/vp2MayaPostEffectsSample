#include "OverridePresentTarget.h"

OverridePresentTarget::OverridePresentTarget(const MString& name) : MPresentTarget(name)
{
	mTargets = NULL;
	mOutputTargetCount = 0;
	mIterationCount = 0;
	mIterationCountEnable = false;
}

OverridePresentTarget::~OverridePresentTarget()
{
	mTargets = NULL;
	mOutputTargetCount = 0;
	mIterationCount = 0;
	mIterationCountEnable = false;
}

MStatus OverridePresentTarget::preProcess()
{
	// printf("OverridePresentTarget::preProcess\n");
	return MStatus::kSuccess;
}

MStatus OverridePresentTarget::postProcess()
{
	// printf("OverridePresentTarget::postProcess\n");
	return MStatus::kSuccess;
}

MHWRender::MRenderTarget* const* OverridePresentTarget::targetOverrideList(unsigned int& listSize)
{
	return getRenderTargets(listSize);
}