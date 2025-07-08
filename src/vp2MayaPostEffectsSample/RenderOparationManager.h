#pragma once

#include <maya/MViewport2Renderer.h>
#include <maya/MRenderTargetManager.h>
#include "CustomRenderProcess.h"
#include <assert.h>
#include <vector>
using namespace amagly;

// Render Operation Manager
class RenderOparationManager
{
public:
    // Render operation types
    enum RenderOparationType
    {
        BaseSceneOparation = 0,
        BlitSceneOparation, // 1
        ClearTempRedBufferOparation, // 2
        RedPassRenderOparation, // 3
        ClearTempBlueBufferOparation, // 4
        BluePassRenderOparation, // 5
        CombinePassRenderOparation, // 6
        PresentTargetOparation, // 7
    };

    RenderOparationManager();
    ~RenderOparationManager();

    MStatus executePreRenderProcess()
    {
        if (mRenderingLoop.size() == 0)
        {
            return MStatus::kFailure;
        }

        CustomRenderProcess *operation = mRenderOperations[mRenderingLoop[mRenderingIndex]];
        return operation->preProcess();
    }

    MStatus executePostRenderProcess()
    {
        if (mRenderingLoop.size() == 0)
        {
            return MStatus::kFailure;
        }

        CustomRenderProcess *operation = mRenderOperations[mRenderingLoop[mRenderingIndex]];
        return operation->postProcess();
    }

    int getRenderOparationCount() const
    {
        return (int)mRenderOperations.size();
    }

    MHWRender::MRenderOperation *renderOparation()
    {
        if (mRenderingLoop.size() == 0)
        {
            return nullptr;
        }

        CustomRenderProcess *operation = mRenderOperations[mRenderingLoop[mRenderingIndex]];
        return operation->renderOperation();
    }

    MStatus setup();
    void start();
    void next()
    {
        mRenderingIndex++;
    }
    void end();
    bool isVaildNextOparation() const
    {
        return mRenderingIndex < (int)mRenderingLoop.size();
    }

protected:
    MStatus initializeShaders();
    MStatus finalizeShaders();
    void addRenderOperation(CustomRenderProcess *operation)
    {
        mRenderOperations.push_back(operation);
    }

    void setRenderingFlow(const std::vector<unsigned int> flow)
    {
        mRenderingLoop = flow;
    }

    std::vector<CustomRenderProcess *> mRenderOperations;
    std::vector<unsigned int> mRenderingLoop;
    unsigned int mRenderingIndex;
};