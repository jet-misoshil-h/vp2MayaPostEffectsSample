#pragma once

#include <maya/MViewport2Renderer.h>
#include "RenderTargetsInterface.h"
#include "CustomRenderProcess.h"
#include "OverrideClearColorInterface.h"

class OverrideQuadRender : public MHWRender::MQuadRender, public RenderTargetsInterface, public amagly::CustomRenderProcess, public amagly::OverrideClearColorInterface
{
public:
	enum EPassType
	{
		kRedPass,
		kBluePass,
		kCombainePass,
		kBlitPass,
		kClearPass
	};

	OverrideQuadRender(const MString &name, unsigned int clearMask);
	~OverrideQuadRender() override;

	// from MRenderOperation
	MHWRender::MRenderTarget *const *targetOverrideList(unsigned int &listSize) override;
	MHWRender::MClearOperation &clearOperation() override;
	// from MQuadRender
	const MHWRender::MShaderInstance *shader() override;

	// from CustomRenderProcess
	MStatus preProcess() override;
	MStatus postProcess() override;
	MHWRender::MRenderOperation *renderOperation() override
	{
		return (MHWRender::MRenderOperation*)this;
	}

	void setPassType(EPassType passType) { mPassType = passType; }
	void setInput1Tex(MHWRender::MRenderTarget *texture) { mTexture1 = texture; }
	void setInput2Tex(MHWRender::MRenderTarget *texture) { mTexture2 = texture; }
	void setInput3Tex(MHWRender::MRenderTarget *texture) { mTexture3 = texture; }

protected:
	MHWRender::MShaderInstance *mShaderInstance;
	MHWRender::MRenderTarget *mTexture1;
	MHWRender::MRenderTarget *mTexture2;
	MHWRender::MRenderTarget *mTexture3;
	MFloatPoint mViewRectangle;
	MString mCustomOutputTargetName;
private:
	MHWRender::MBlendState *mBlendState;
	EPassType mPassType;
};