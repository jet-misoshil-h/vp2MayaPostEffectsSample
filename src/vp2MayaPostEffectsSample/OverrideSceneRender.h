#pragma once

#include <maya/MViewport2Renderer.h>
#include <maya/MFnCamera.h>
#include "RenderTargetsInterface.h"
#include "CustomRenderProcess.h"
#include "OverrideClearColorInterface.h"

class OverrideSceneRender : public MHWRender::MSceneRender, public RenderTargetsInterface, public amagly::CustomRenderProcess, public amagly::OverrideClearColorInterface
{
public:
    OverrideSceneRender(const MString &name, unsigned int clearMask);
    ~OverrideSceneRender() override;

    // from MRenderOperation
    MRenderTarget *const *targetOverrideList(unsigned int &listSize) override;
    MHWRender::MClearOperation &clearOperation() override;
    // from MSceneRender
    const MFloatPoint *viewportRectangleOverride() override;

    void setViewRectangle(const MFloatPoint &rect) { mViewRectangle = rect; }
    void setSceneFilterOption(MHWRender::MSceneRender::MSceneFilterOption sceneFilter);
    MHWRender::MSceneRender::MSceneFilterOption renderFilterOverride() override;
    MHWRender::MSceneRender::MDisplayMode displayModeOverride() override;

    const MString &panelName() const
    {
        return mPanelName;
    }
    void setPanelName(const MString &name)
    {
        mPanelName.set(name.asChar());
    }

    MStatus preProcess() override;
    MStatus postProcess() override;
    MHWRender::MRenderOperation *renderOperation() override
    {
        return (MHWRender::MRenderOperation*)this;
    }

protected:
    MHWRender::MCameraOverride *SetDefaultCamera();

        // 3D viewport panel name, if available
    MString mPanelName;
    MFloatPoint mViewRectangle;
    // Scene draw filter override
    MHWRender::MSceneRender::MSceneFilterOption mSceneFilter;
    bool mOverrideDisplayMode;
    bool mOverrideDisplayModeFlatActive;
};
