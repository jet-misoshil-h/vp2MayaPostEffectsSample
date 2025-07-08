#include "FrameTextureManager.h"
#include <maya/MTextureManager.h>

namespace amagly
{
    FrameTextureManager *FrameTextureManager::instance_ = nullptr;

    FrameTextureManager::FrameTextureManager()
    {
        for (unsigned int targetId = 0; targetId < kBufferCount; ++targetId)
        {
            mTargetDescriptions[targetId] = nullptr;
            mTargets[targetId] = nullptr;
            mTargetSupportsSRGBWrite[targetId] = false;
        }

        unsigned int sampleCount = 1;
        MHWRender::MRasterFormat colorFormat = MHWRender::kR8G8B8A8_UNORM;
        MHWRender::MRasterFormat depthFormat = MHWRender::kD24S8;

        mTargetDescriptions[kColorBuffer] = new MHWRender::MRenderTargetDescription(mColorBufferName, 512, 512, sampleCount, colorFormat, 0, false);
        mTargets[kColorBuffer] = NULL;
        mTargetSupportsSRGBWrite[kColorBuffer] = false;

        mTargetDescriptions[kDepthBuffer] = new MHWRender::MRenderTargetDescription(mDepthBufferName, 512, 512, sampleCount, depthFormat, 0, false);
        mTargets[kDepthBuffer] = NULL;
        mTargetSupportsSRGBWrite[kDepthBuffer] = false;

        mTargetDescriptions[kTempBufferRed] = new MHWRender::MRenderTargetDescription(mTempRedName, 512, 512, sampleCount, depthFormat, 0, false);
        mTargets[kTempBufferRed] = NULL;
        mTargetSupportsSRGBWrite[kTempBufferRed] = false;

        mTargetDescriptions[kTempBufferBlue] = new MHWRender::MRenderTargetDescription(mTempBlueName, 512, 512, sampleCount, depthFormat, 0, false);
        mTargets[kTempBufferBlue] = NULL;
        mTargetSupportsSRGBWrite[kTempBufferBlue] = false;

        mTargetDescriptions[kTempColorBuffer] = new MHWRender::MRenderTargetDescription(mTempColorBufferName, 512, 512, sampleCount, colorFormat, 0, false);
        mTargets[kTempColorBuffer] = NULL;
        mTargetSupportsSRGBWrite[kTempColorBuffer] = false;
    }

    FrameTextureManager::~FrameTextureManager()
    {
        ReleaseTempBuffers();
    }

    MHWRender::MRenderTarget **FrameTextureManager::GetColorBuffer()
    {
        return GetTempBufferById(kColorBuffer);
    }

    MHWRender::MRenderTarget **FrameTextureManager::GetDepthBuffer()
    {
        return GetTempBufferById(kDepthBuffer);
    }

    MHWRender::MRenderTarget **FrameTextureManager::GetTempRedBuffer()
    {
        return GetTempBufferById(kTempBufferRed);
    }

    MHWRender::MRenderTarget **FrameTextureManager::GetTempBlueBuffer()
    {
        return GetTempBufferById(kTempBufferBlue);
    }

    MHWRender::MRenderTarget **FrameTextureManager::GetTempColorBuffer()
    {
        return GetTempBufferById(kTempColorBuffer);
    }

    MStatus FrameTextureManager::UpdateRenderTargets()
    {
        MHWRender::MRenderer *theRenderer = MHWRender::MRenderer::theRenderer();

        // Get the current output target size as specified by the
        // renderer. If it has changed then the targets need to be
        // resized to match.
        unsigned int targetWidth = 0;
        unsigned int targetHeight = 0;
        if (theRenderer)
        {
            theRenderer->outputTargetSize(targetWidth, targetHeight);
        }
        else
        {
            return MStatus::kFailure;
        }

        // Note that the render target sizes could be set to be
        // smaller than the size used by the renderer. In this case
        // a final present will generally stretch the output.

        const MHWRender::MRenderTargetManager *targetManager = theRenderer ? theRenderer->getRenderTargetManager() : NULL;
        if (targetManager)
        {
            for (unsigned int targetId = 0; targetId < kBufferCount; ++targetId)
            {
                if (mTargetDescriptions[targetId])
                {
                    // if (targetId == kTempBufferBlue || targetId == kTempBufferRed)
                    // {
                    //     mTargetDescriptions[targetId]->setWidth(targetWidth / 2);
                    //     mTargetDescriptions[targetId]->setHeight(targetHeight / 2);
                    // }
                    // else
                    // {
                    //     mTargetDescriptions[targetId]->setWidth(targetWidth);
                    //     mTargetDescriptions[targetId]->setHeight(targetHeight);
                    // }
                    mTargetDescriptions[targetId]->setWidth(targetWidth);
                    mTargetDescriptions[targetId]->setHeight(targetHeight);

                    if (!mTargetSupportsSRGBWrite[targetId])
                    {
                        mTargetSupportsSRGBWrite[targetId] = targetManager->formatSupportsSRGBWrite(mTargetDescriptions[targetId]->rasterFormat());
                    }
                    mTargetDescriptions[targetId]->setAllowsUnorderedAccess(false);

                    if (!mTargets[targetId])
                    {
                        mTargets[targetId] = targetManager->acquireRenderTarget(*(mTargetDescriptions[targetId]));
                        // printf("%s : %s", "acquireRenderTarget", mTargetDescriptions[targetId]->name().asChar());
                        // printf(mTargets[targetId] ? " => success\n" : " => failed\n");
                    }
                    else
                    {
                        mTargets[targetId]->updateDescription(*(mTargetDescriptions[targetId]));
                    }
                }
            }

            for (int i = 0; i < kBufferCount; i++)
            {
                if (!mTargets[i])
                {
                    return MStatus::kFailure;
                }
            }

            return MStatus::kSuccess;
        }

        printf("Target manager not available yet\n");
        return MStatus::kFailure;
    }

    void FrameTextureManager::ReleaseTempBuffers()
    {
        MHWRender::MRenderer *theRenderer = MHWRender::MRenderer::theRenderer();

        const MHWRender::MRenderTargetManager *targetManager = theRenderer ? theRenderer->getRenderTargetManager() : NULL;
        if (targetManager)
        {
            for (unsigned int targetId = 0; targetId < kBufferCount; ++targetId)
            {

                if (mTargetDescriptions[targetId])
                {
                    delete mTargetDescriptions[targetId];
                    mTargetDescriptions[targetId] = NULL;
                }

                if (mTargets[targetId])
                {
                    targetManager->releaseRenderTarget(mTargets[targetId]);
                }
                mTargets[targetId] = nullptr;
            }
        }
        else
        {
            printf("Target manager not available yet\n");
        }
    }
}