#pragma once

#include <maya/MViewport2Renderer.h>
#include <maya/MRenderTargetManager.h>
#include <assert.h>

namespace amagly
{
    // Texture Manager for frame render texture
    class FrameTextureManager
    {
    public:
        enum EBufferId
        {
            kColorBuffer,
            kDepthBuffer,
            kTempBufferRed,
            kTempBufferBlue,
            kTempColorBuffer,

            kBufferCount
        };

        static void CreateInstance()
        {
            assert(!instance_);
            instance_ = new FrameTextureManager();
        }
        static void DestroyInstance()
        {
            assert(instance_);
            delete instance_;
        }
        static FrameTextureManager *Get() { return instance_; }
        MHWRender::MRenderTarget **GetColorBuffer();
        MHWRender::MRenderTarget **GetDepthBuffer();
        MHWRender::MRenderTarget **GetTempRedBuffer();
        MHWRender::MRenderTarget **GetTempBlueBuffer();
        MHWRender::MRenderTarget **GetTempColorBuffer();

        MHWRender::MRenderTarget **GetTempBufferById(EBufferId id)
        {
            MHWRender::MRenderTarget **buffers[1];
            buffers[0] = &mTargets[id];
            return *buffers;
        }

        MHWRender::MRenderTarget **GetColorDepthBuffers()
        {
            MHWRender::MRenderTarget **buffers[2];
            buffers[0] = &mTargets[kColorBuffer];
            buffers[1] = &mTargets[kDepthBuffer];
            return *buffers;
        }
        MHWRender::MRenderTarget **GetBuffers() { return mTargets; }
        MHWRender::MRenderTarget *const *GetBuffer(MString name) const
        {
            for (unsigned int i = 0; i < kBufferCount; ++i)
            {
                if (mTargetDescriptions[i] && mTargetDescriptions[i]->name() == name)
                {
                    return &mTargets[i];
                }
            }
            return nullptr;
        }

        // Update the temporary buffer
        MStatus UpdateRenderTargets();
        // Release the temporary buffer
        void ReleaseTempBuffers();

        MHWRender::MRenderTargetDescription *mTargetDescriptions[kBufferCount];
        MHWRender::MRenderTarget *mTargets[kBufferCount];
        bool mTargetSupportsSRGBWrite[kBufferCount];

        const MString mColorBufferName = "__vp2AMGPostEffectColorBuffer__";
        const MString mDepthBufferName = "__vp2AMGPostEffectDepthBuffer__";
        const MString mTempRedName = "__vp2AMGPostEffectRedBuffer__";
        const MString mTempBlueName = "__vp2AMGPostEffectBlueBuffer__";
        const MString mTempColorBufferName = "__vp2AMGPostEffectTempColorBuffer__";

    private:
        FrameTextureManager();
        ~FrameTextureManager();

        static FrameTextureManager *instance_;
    };
}