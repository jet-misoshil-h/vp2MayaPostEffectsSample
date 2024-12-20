#include <maya/MRenderTargetManager.h>
#include <maya/MGlobal.h>
#include <maya/M3dView.h>
#include <maya/MRenderTargetManager.h>
#include <maya/MTextureManager.h>
#include <maya/MDrawContext.h>
#include "OverrideSceneRender.h"
#include "FrameTextureManager.h"
using namespace amagly;

OverrideSceneRender::OverrideSceneRender(
	const MString &name, unsigned int clearMask) : MHWRender::MSceneRender(name),
														OverrideClearColorInterface(clearMask)
{
	// Set the default scene filter option
	mSceneFilter = MHWRender::MSceneRender::kNoSceneFilterOverride;
	mOverrideDisplayMode = false;
	mOverrideDisplayModeFlatActive = false;
	mOverrideClearColor = false;
	mClearColor = MColor(0.0f, 0.0f, 0.0f, 1.0f);

	mTargets = NULL;
	mOutputTargetCount = 0;
	mViewRectangle[0] = 0.0f;
	mViewRectangle[1] = 0.0f;
	mViewRectangle[2] = 1.0f;
	mViewRectangle[3] = 1.0f;

	mIterationCount = 0;
	mIterationCountEnable = false;
}

OverrideSceneRender::~OverrideSceneRender()
{
	mTargets = NULL;
	mOutputTargetCount = 0;
}

MStatus OverrideSceneRender::preProcess()
{
	return MStatus::kSuccess;
}

MStatus OverrideSceneRender::postProcess()
{
	return MStatus::kSuccess;
}

MHWRender::MRenderTarget *const *OverrideSceneRender::targetOverrideList(unsigned int &listSize)
{
	return getRenderTargets(listSize);
}

MHWRender::MClearOperation &OverrideSceneRender::clearOperation()
{
	MHWRender::MRenderer *renderer = MHWRender::MRenderer::theRenderer();
	bool gradient = renderer->useGradient();

	float c1[4];
	float c2[4];

	if (mOverrideClearColor)
	{
		gradient = true;
		c1[0] = c2[0] = mClearColor.r;
		c1[1] = c2[1] = mClearColor.g;
		c1[2] = c2[2] = mClearColor.b;
		c1[3] = c2[3] = mClearColor.a;
	}
	else
	{
		MColor color1 = renderer->clearColor();
		MColor color2 = renderer->clearColor2();

		c1[0] = color1.r;
		c1[1] = color1.g;
		c1[2] = color1.b;
		c1[3] = 1.0f;
		c2[0] = color2.r;
		c2[1] = color2.g;
		c2[2] = color2.b;
		c2[3] = 1.0f;
	}

	mClearOperation.setClearColor(c1);
	mClearOperation.setClearColor2(c2);
	mClearOperation.setClearGradient(gradient);
	mClearOperation.setMask(mClearMask);

	return mClearOperation;
}

void OverrideSceneRender::setSceneFilterOption(MHWRender::MSceneRender::MSceneFilterOption sceneFilter)
{
	// Set the scene filter option
	mSceneFilter = sceneFilter;
}

/*
	Depending on what is required either the scene filter will return whether
	to draw the opaque, transparent of non-shaded (UI) items.
*/
MHWRender::MSceneRender::MSceneFilterOption
OverrideSceneRender::renderFilterOverride()
{
	return mSceneFilter;
}

/*
	Example display mode override. In this example we override so that
	the scene will always be drawn in "shaded" or "flat shade selected" mode
	and in bounding box mode (bounding boxes will also be drawn). This is fact not a
	'regular' viewport display mode available from the viewport menus.
*/
MHWRender::MSceneRender::MDisplayMode OverrideSceneRender::displayModeOverride()
{
	if (mOverrideDisplayMode)
	{
		if (mOverrideDisplayModeFlatActive)
		{
			return (MHWRender::MSceneRender::MDisplayMode)(MHWRender::MSceneRender::kBoundingBox |
														   MHWRender::MSceneRender::kFlatShaded |
														   MHWRender::MSceneRender::kShadeActiveOnly);
		}
		else
		{
			return (MHWRender::MSceneRender::MDisplayMode)(MHWRender::MSceneRender::kBoundingBox |
														   MHWRender::MSceneRender::kShaded);
		}
	}
	return MHWRender::MSceneRender::kNoDisplayModeOverride;
}

const MFloatPoint *OverrideSceneRender::viewportRectangleOverride()
{
	// Return the rectangle for the quad render
	return &mViewRectangle;
}
