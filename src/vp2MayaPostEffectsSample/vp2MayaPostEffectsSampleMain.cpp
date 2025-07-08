#include <maya/MFnPlugin.h>
#include "vp2MayaPostEffectsSample.h"

vp2MayaPostEffectsSample *gRenderOverride = nullptr;

MStatus initializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj, "jet_misoshil", "1.0.0", "Any");

    if (!gRenderOverride)
    {
        gRenderOverride = new vp2MayaPostEffectsSample("vp2MayaPostEffectsSample");
        MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
        if (renderer)
        {
            status = renderer->registerOverride(gRenderOverride);
        }
    }

    if (status != MStatus::kSuccess)
    {
        status.perror("registerOverride");
    }

    return status;
}

MStatus uninitializePlugin(MObject obj)
{
    MStatus status;
    MFnPlugin plugin(obj);

    if (gRenderOverride)
    {
        MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
        if (renderer)
        {
            status = renderer->deregisterOverride(gRenderOverride);
        }

        delete gRenderOverride;
        gRenderOverride = nullptr;
    }

    if (status != MStatus::kSuccess)
    {
        status.perror("deregisterOverride");
    }

    return status;
}