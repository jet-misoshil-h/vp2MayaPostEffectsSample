#include <maya/MShaderManager.h>
#include "OverrideQuadRender.h"
#include "FrameTextureManager.h"
using namespace amagly;

OverrideQuadRender::OverrideQuadRender(const MString &name, unsigned int clearMask)
	: MQuadRender(name), OverrideClearColorInterface(clearMask), mShaderInstance(nullptr), mTexture1(nullptr), mTexture2(nullptr), mTexture3(nullptr)
{
	mOverrideClearColor = false;
	mClearColor = MColor(0.0f, 0.0f, 0.0f, 1.0f);
	mTargets = NULL;
	mOutputTargetCount = 0;
	mIterationCount = 0;
	mIterationCountEnable = false;
	mCustomOutputTargetName = kColorTargetName;
}

OverrideQuadRender::~OverrideQuadRender()
{
	mTexture1 = nullptr;
	mTexture2 = nullptr;
	mTexture3 = nullptr;
	if (mShaderInstance)
	{
		MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
		if (renderer)
		{
			const MHWRender::MShaderManager *shaderMgr = renderer->getShaderManager();
			if (shaderMgr)
			{
				shaderMgr->releaseShader(mShaderInstance);
			}
		}
		mShaderInstance = nullptr;
	}
	mTargets = NULL;
	mOutputTargetCount = 0;
}

MStatus OverrideQuadRender::preProcess()
{
	// printf("%s::preProcess\n", name().asChar());
	if (mIterationCountEnable)
	{
		if (mIterationCount == 0)
		{
			// if last iteration, not clear the render target
			mClearMask = (unsigned int)MHWRender::MClearOperation::kClearNone;
		}
	}

	return MStatus::kSuccess;
}

MStatus OverrideQuadRender::postProcess()
{
	// printf("%s::postProcess\n", name().asChar());
	if (mIterationCountEnable)
	{
		// printf("IterationCount Before : %d\n", mIterationCount);
		if (mIterationCount > 0)
		{
			mIterationCount--;
			// printf("IterationCount After : %d\n", mIterationCount);
		}
	}
	return MStatus::kSuccess;
}



MHWRender::MRenderTarget *const *OverrideQuadRender::targetOverrideList(unsigned int &listSize)
{
	// printf("%s::targetOverrideList\n", name().asChar());
	return getRenderTargets(listSize);
}

MHWRender::MClearOperation &OverrideQuadRender::clearOperation()
{
	// printf("%s::clearOperation\n", name().asChar());
	if (mOverrideClearColor)
	{
		float c1[4];
		float c2[4];
		c1[0] = c2[0] = mClearColor.r;
		c1[1] = c2[1] = mClearColor.g;
		c1[2] = c2[2] = mClearColor.b;
		c1[3] = c2[3] = mClearColor.a;

		mClearOperation.setClearColor(c1);
		mClearOperation.setClearColor2(c2);
		mClearOperation.setClearGradient(false);
		mClearOperation.setMask(mClearMask);
		return mClearOperation;
	}

	mClearOperation.setClearGradient(false);
	mClearOperation.setMask(mClearMask);
	return mClearOperation;
}

const MHWRender::MShaderInstance *OverrideQuadRender::shader()
{
	// printf("%s::shader\n", name().asChar());
	if (mShaderInstance == nullptr)
	{
		MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
		if (renderer)
		{
			const MHWRender::MShaderManager *shaderMgr = renderer->getShaderManager();
			if (shaderMgr)
			{
				switch (mPassType)
				{
				case EPassType::kRedPass:
					mShaderInstance = shaderMgr->getEffectsFileShader("PostEffectSample", "Shift", nullptr, 0, false);
					break;
				case EPassType::kBluePass:
					mShaderInstance = shaderMgr->getEffectsFileShader("PostEffectSample", "BlueShift", nullptr, 0, false);
					break;
				case EPassType::kCombainePass:
					mShaderInstance = shaderMgr->getEffectsFileShader("PostEffectSample", "Combine", nullptr, 0, false);
					break;
				case EPassType::kBlitPass:
					mShaderInstance = shaderMgr->getEffectsFileShader("PostEffectSample", "Blit", nullptr, 0, false);
					break;
				case EPassType::kClearPass:
					mShaderInstance = shaderMgr->getEffectsFileShader("PostEffectSample", "Clear", nullptr, 0, false);
					return mShaderInstance;
				default:
					return nullptr;
				}
			}
		}
	}

	if (mShaderInstance)
	{
		// printf("ShaderInstance : %s\n", name().asChar());
		// Set the input texture
		{
			MHWRender::MRenderTargetAssignment assignment;
			assignment.target = mTexture1;
			MStatus status = mShaderInstance->setParameter("gInputTex1", assignment);
			if (status != MStatus::kSuccess)
			{
				printf("Could not set input render target / texture parameter on invert shader\n");
				return nullptr;
			}
		}

		switch (mPassType)
		{
		case EPassType::kRedPass:
		case EPassType::kBluePass:
		{
			if (mTexture2)
			{
				// Set Depth Texture
				MHWRender::MRenderTargetAssignment assignment;
				assignment.target = mTexture2;
				MStatus status = mShaderInstance->setParameter("gInputTex2", assignment);
				if (status != MStatus::kSuccess)
				{
					printf("Could not set input render target / texture parameter on invert shader\n");
					return nullptr;
				}
			}
			break;
		}
		case EPassType::kCombainePass:
		{
			// Set the second input texture
			if (mTexture2)
			{
				MHWRender::MRenderTargetAssignment assignment;
				assignment.target = mTexture2;
				MStatus status = mShaderInstance->setParameter("gInputTex2", assignment);
				if (status != MStatus::kSuccess)
				{
					printf("Could not set input render target / texture parameter on invert shader\n");
					return nullptr;
				}
			}

			// Set the third input texture
			if (mTexture3)
			{
				MHWRender::MRenderTargetAssignment assignment;
				assignment.target = mTexture3;
				MStatus status = mShaderInstance->setParameter("gInputTex3", assignment);
				if (status != MStatus::kSuccess)
				{
					printf("Could not set input render target / texture parameter on invert shader\n");
					return nullptr;
				}
			}
			break;
		}
		case EPassType::kBlitPass:
		case EPassType::kClearPass:
		default:
			break;
		}
	}

	return mShaderInstance;
}