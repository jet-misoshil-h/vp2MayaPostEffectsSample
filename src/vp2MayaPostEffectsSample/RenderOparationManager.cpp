#include "RenderOparationManager.h"
#include "OverrideSceneRender.h"
#include "OverridePresentTarget.h"
#include "OverrideQuadRender.h"
#include "RenderTargetsInterface.h"
#include <maya/MGlobal.h>
#include <maya/MShaderManager.h>
#include "FrameTextureManager.h"
#include "Common.h"
#include <sys/stat.h>
using namespace amagly;

RenderOparationManager::RenderOparationManager() : mRenderingIndex(0)
{
    mRenderingLoop.clear();
    mRenderOperations.clear();
    MStatus stat = initializeShaders();
    if (stat == MStatus::kSuccess)
    {
        MGlobal::displayInfo("Initialize RenderOparationManager");
    }
    else
    {
        MGlobal::displayError("Failed to initialize shaders");
    }
}

RenderOparationManager::~RenderOparationManager()
{
    for (int i = 0; i < mRenderOperations.size(); ++i)
    {
        if (mRenderOperations[i])
        {
            delete mRenderOperations[i];
        }
        mRenderOperations[i] = nullptr;
    }
    mRenderOperations.clear();
    mRenderingLoop.clear();
    mRenderingIndex = 0;
    MStatus stat = finalizeShaders();
    if (stat == MStatus::kSuccess)
    {
        MGlobal::displayInfo("Finalize RenderOparationManager");
    }
    else
    {
        MGlobal::displayError("Failed to finalize shaders");
    }
}

MStatus RenderOparationManager::setup()
{
    auto *frameTexManager = static_cast<FrameTextureManager *>(FrameTextureManager::Get());
    MStatus stat = frameTexManager->UpdateRenderTargets();
    if (stat == MStatus::kFailure)
    {
        MGlobal::displayError("Failed to Update RenderTargets");
        return MStatus::kFailure;
    }

    if (mRenderOperations.size() == 0)
    {
        // -----------------------------------
        // inisialize the rendering oparations
        // -----------------------------------

        // Base Scene Render: 0
        OverrideSceneRender *baseSceneOp = new OverrideSceneRender(
            "vp2MayaPostEffectsSampleSceneRender",
            (unsigned int)(MHWRender::MClearOperation::kClearAll));
        baseSceneOp->setRenderTargets(frameTexManager->GetColorDepthBuffers());
        baseSceneOp->setTargetCount(2);
        addRenderOperation((CustomRenderProcess *)baseSceneOp);
        // printf("addRenderOperation baseSceneOp\n");

        // Blit the scene to a smaller buffer: 1
        {
            OverrideQuadRender *blitScene = new OverrideQuadRender("vp2MayaPostEffectsSampleBlitScene", (unsigned int)(MHWRender::MClearOperation::kClearNone));
            blitScene->setPassType(OverrideQuadRender::kBlitPass);
            blitScene->setInput1Tex(frameTexManager->GetBuffers()[FrameTextureManager::kColorBuffer]);
            blitScene->setRenderTargets(frameTexManager->GetTempColorBuffer());
            blitScene->setTargetCount(1);
            blitScene->enableOverrideClearColor(MColor(0.0f, 0.0f, 0.0f, 0.0f));
            addRenderOperation((CustomRenderProcess *)blitScene);
            // printf("addRenderOperation blitScene\n");
        }

        // Post Effects
        {
            // Red Clear Pass: 2
            OverrideQuadRender *clearTemp = new OverrideQuadRender("vp2MayaPostEffectsSampleClearTempRed", (unsigned int)(MHWRender::MClearOperation::kClearNone));
            clearTemp->setPassType(OverrideQuadRender::kClearPass);
            clearTemp->setRenderTargets(frameTexManager->GetTempRedBuffer());
            clearTemp->setTargetCount(1);
            addRenderOperation((CustomRenderProcess *)clearTemp);

            // Red Pass: 3
            OverrideQuadRender *postEPass = new OverrideQuadRender("vp2MayaPostEffectsSampleRedPass", (unsigned int)(MHWRender::MClearOperation::kClearNone));
            postEPass->setPassType(OverrideQuadRender::kRedPass);
            postEPass->setInput1Tex(frameTexManager->GetBuffers()[FrameTextureManager::kTempColorBuffer]);
            postEPass->setRenderTargets(frameTexManager->GetColorBuffer());
            postEPass->setTargetCount(1);
            addRenderOperation((CustomRenderProcess *)postEPass);
            // printf("addRenderOperation postEPass : RedPass\n");
        }

        {
            // Blue Clear Pass:@ 4
            OverrideQuadRender *clearTemp = new OverrideQuadRender("vp2MayaPostEffectsSampleClearTempBlue", (unsigned int)(MHWRender::MClearOperation::kClearNone));
            clearTemp->setPassType(OverrideQuadRender::kClearPass);
            clearTemp->setRenderTargets(frameTexManager->GetTempBlueBuffer());
            clearTemp->setTargetCount(1);
            addRenderOperation((CustomRenderProcess *)clearTemp);

            // Blue Pass: 5
            OverrideQuadRender *postEPass = new OverrideQuadRender("vp2MayaPostEffectsSampleBluePass", (unsigned int)(MHWRender::MClearOperation::kClearNone));
            postEPass->setPassType(OverrideQuadRender::kBluePass);
            postEPass->setInput1Tex(frameTexManager->GetBuffers()[FrameTextureManager::kTempColorBuffer]);
            postEPass->setRenderTargets(frameTexManager->GetColorBuffer());
            postEPass->setTargetCount(1);
            addRenderOperation((CustomRenderProcess *)postEPass);
            // printf("addRenderOperation postEPass : BluePass\n");
        }

        {
            // Combine Pass: 6
            OverrideQuadRender *postEPass = new OverrideQuadRender("vp2MayaPostEffectsSampleCombinePass", (unsigned int)(MHWRender::MClearOperation::kClearNone));
            postEPass->setPassType(OverrideQuadRender::kCombainePass);
            postEPass->setInput1Tex(frameTexManager->GetBuffers()[FrameTextureManager::kTempColorBuffer]);
            postEPass->setInput2Tex(frameTexManager->GetBuffers()[FrameTextureManager::kTempBufferRed]);
            postEPass->setInput3Tex(frameTexManager->GetBuffers()[FrameTextureManager::kTempBufferBlue]);
            postEPass->setRenderTargets(frameTexManager->GetColorBuffer());
            postEPass->setTargetCount(1);
            addRenderOperation((CustomRenderProcess *)postEPass);
            // printf("addRenderOperation postEPass : CombinePass\n");
        }

        // Present target render backbuffer: 7
        OverridePresentTarget *overridePresentTarget = new OverridePresentTarget("vp2MayaPostEffectsSamplePresent");
        overridePresentTarget->setRenderTargets(frameTexManager->GetColorDepthBuffers());
        overridePresentTarget->setTargetCount(2);
        addRenderOperation((CustomRenderProcess *)overridePresentTarget);
        // printf("addRenderOperation overridePresentTarget\n");

        // Set the rendering flow
        std::vector<unsigned int> renderingFlow;
        renderingFlow = {
            RenderOparationType::BaseSceneOparation,
            RenderOparationType::BlitSceneOparation,
            // RenderOparationType::ClearTempRedBufferOparation,
            RenderOparationType::RedPassRenderOparation,
            // RenderOparationType::ClearTempBlueBufferOparation,
            // RenderOparationType::BluePassRenderOparation,
            // RenderOparationType::CombinePassRenderOparation,
            RenderOparationType::PresentTargetOparation
        };
        setRenderingFlow(renderingFlow);
        // printf("setup renderingFlow\n");
    }

    for (int i = 0; i < mRenderOperations.size(); i++)
    {
        if (!mRenderOperations[i])
        {
            return MStatus::kFailure;
        }
    }

    // printf("setuped RenderOparationManager\n");
    return MStatus::kSuccess;
}

// Initialize the shader
MStatus RenderOparationManager::initializeShaders()
{
    MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
    if (renderer)
    {
        printf("initializeShaders\n");
        const MHWRender::MShaderManager *shaderMgr = renderer->getShaderManager();
        if (shaderMgr)
        {
            // Get the path to the shader
            struct stat info;
            const MString shaderPath = MString("${AMG_SAMPLE_SHADER_PATH}").expandEnvironmentVariablesAndTilde();
            if (stat(shaderPath.asChar(), &info) != 0)
            {
                MDisplayError("Failed to get shader path : %s", shaderPath.asChar());
                return MStatus::kFailure;
            }
            else
            {
                MDisplayInfo("Find to shader path : %s", shaderPath.asChar());
                printf("%s : %s%s", "Find to shader path", shaderPath.asChar(), "\n");
                shaderMgr->addShaderPath(shaderPath);
                shaderMgr->addShaderIncludePath(shaderPath);
                return MStatus::kSuccess;
            }
        }
    }

    return MStatus::kFailure;
}

MStatus RenderOparationManager::finalizeShaders()
{
    MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
    if (renderer)
    {
        printf("finalizeShaders\n");
        const MHWRender::MShaderManager *shaderMgr = renderer->getShaderManager();
        if (shaderMgr)
        {
            // Get the path to the shader
            const MString shaderPath = MString("${AMG_SAMPLE_SHADER_PATH}").expandEnvironmentVariablesAndTilde();

            printf("%s : %s%s", "Remove shader path", shaderPath.asChar(), "\n");
            MStringArray paths;
            MStringArray includePaths;
            shaderMgr->shaderPaths(paths);
            shaderMgr->shaderIncludePaths(includePaths);
            int pathIndex = -1;

            for (unsigned int i = 0; i < paths.length(); ++i)
            {
                if (paths[i] == shaderPath)
                {
                    paths[i] = nullptr;
                    includePaths[i] = nullptr;
                    pathIndex = i;
                    break;
                }
            }

            if (pathIndex != -1)
            {
                shaderMgr->addShaderPath(shaderPath);
                shaderMgr->addShaderIncludePath(shaderPath);
                return MStatus::kSuccess;
            }
        }
    }

    return MStatus::kFailure;
}

void RenderOparationManager::start()
{
    // printf("RenderOparationManager::start\n");
    mRenderingIndex = 0;
}

void RenderOparationManager::end()
{
    // printf("RenderOparationManager::end\n");
    mRenderingIndex = 0;
}