#pragma once

#include <maya/MViewport2Renderer.h>
#include "RenderOparationManager.h"
#include <vector>

class vp2MayaPostEffectsSample : public MHWRender::MRenderOverride {
public:
    vp2MayaPostEffectsSample(const MString& name);
    ~vp2MayaPostEffectsSample() override;

    MHWRender::DrawAPI supportedDrawAPIs() const override;

    bool startOperationIterator() override;
    MHWRender::MRenderOperation* renderOperation() override;
    bool nextRenderOperation() override;

    MString uiName() const override { return mUIName; }

    MStatus setup(const MString& destination) override;
    MStatus cleanup() override;

protected:
    MString mUIName;
    RenderOparationManager* mRenderOparationManager;
};
