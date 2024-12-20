#pragma once
#include <maya/MViewport2Renderer.h>

namespace amagly
{
    // Override ClearColor Interface
    class OverrideClearColorInterface
    {
    public:
        OverrideClearColorInterface(unsigned int clearMask);
        ~OverrideClearColorInterface();

        void enableOverrideClearColor(const MColor &color)
        {
            mOverrideClearColor = true;
            mClearColor = color;
        }
        void desableOverrideClearColor() { mOverrideClearColor = false; }

    protected:
        unsigned int mClearMask;
        bool mOverrideClearColor;
        MColor mClearColor;
    };
}