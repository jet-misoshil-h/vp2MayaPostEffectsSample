#pragma once

#include <maya/MViewport2Renderer.h>

namespace amagly
{
    // Custom Render Process Interface
    class CustomRenderProcess
    {
    public:
        CustomRenderProcess() : mIterationCount(0), mIterationCountEnable(false) {};
        virtual ~CustomRenderProcess() {};

        virtual MStatus preProcess(){ return MStatus::kSuccess; }
        virtual MStatus postProcess(){ return MStatus::kSuccess; }
        virtual MHWRender::MRenderOperation *renderOperation() { return nullptr; }
        virtual void setIterationCount(unsigned int count)
        {
            mIterationCount = count;
            mIterationCountEnable = true;
        }

    protected:
        unsigned int mIterationCount;
        bool mIterationCountEnable;
    };
}