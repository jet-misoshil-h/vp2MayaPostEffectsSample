#include "vp2MayaPostEffectsSample.h"
#include "FrameTextureManager.h"
#include <maya/MGlobal.h>


vp2MayaPostEffectsSample::vp2MayaPostEffectsSample(const MString &name)
	: MHWRender::MRenderOverride(name),
	  mUIName("vp2MayaPostEffectsSample")
{
	MGlobal::displayInfo("start initialize vp2MayaPostEffectsSample");
	// Create the FrameTextureManager instance
	amagly::FrameTextureManager::CreateInstance();
	printf("FrameTextureManager::CreateInstance()\n");
	mRenderOparationManager = new RenderOparationManager();
	printf("new RenderOparationManager()\n");
	MGlobal::displayInfo("Initialized vp2MayaPostEffectsSample");
}

vp2MayaPostEffectsSample::~vp2MayaPostEffectsSample()
{
	amagly::FrameTextureManager::DestroyInstance();
	if (mRenderOparationManager)
	{
		delete mRenderOparationManager;
	}
	mRenderOparationManager = nullptr;
	MGlobal::displayInfo("vp2MayaPostEffectsSample Finalized");
}

MHWRender::DrawAPI vp2MayaPostEffectsSample::supportedDrawAPIs() const
{
	return MHWRender::kDirectX11;
}

bool vp2MayaPostEffectsSample::startOperationIterator()
{
	mRenderOparationManager->start();
	return true;
}

MHWRender::MRenderOperation *vp2MayaPostEffectsSample::renderOperation()
{
	MStatus status = mRenderOparationManager->executePreRenderProcess();
	if (status != MStatus::kSuccess)
	{
		printf("Could not execute pre render process\n");
		return nullptr;
	}
	return mRenderOparationManager->renderOparation();
}

bool vp2MayaPostEffectsSample::nextRenderOperation()
{
	MStatus status = mRenderOparationManager->executePostRenderProcess();
	if (status != MStatus::kSuccess)
	{
		printf("Could not execute post render process\n");
		return false;
	}
	mRenderOparationManager->next();
	return mRenderOparationManager->isVaildNextOparation();
}

MStatus vp2MayaPostEffectsSample::setup(const MString &destination)
{
	MHWRender::MRenderer *theRenderer = MHWRender::MRenderer::theRenderer();
	if (!theRenderer)
	{
		return MStatus::kFailure;
	}

	return mRenderOparationManager->setup();
}

MStatus vp2MayaPostEffectsSample::cleanup()
{
	return MStatus::kSuccess;
}