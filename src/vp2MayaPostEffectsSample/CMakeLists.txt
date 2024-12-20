#-
# ==========================================================================
# Copyright (c) 2024 jet-misoshil-h.
# All rights reserved.
# ==========================================================================
#+


cmake_minimum_required(VERSION 3.13)

# set env variable DEVKIT_LOCATION to the location of Maya devkit
set(ENV{DEVKIT_LOCATION} C:/work/maya_devkit/devkitBase)

# include the project setting file
include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake)

# specify project name
set(PROJECT_NAME vp2MayaPostEffectsSample)

# set SOURCE_FILES
set(SOURCE_FILES
Common.h
CustomRenderProcess.h
FrameTextureManager.cpp
FrameTextureManager.h
OverrideClearColorInterface.cpp
OverrideClearColorInterface.h
OverridePresentTarget.cpp
OverridePresentTarget.h
OverrideQuadRender.cpp
OverrideQuadRender.h
OverrideSceneRender.cpp
OverrideSceneRender.h
RenderOparationManager.cpp
RenderOparationManager.h
RenderTargetsInterface.cpp
RenderTargetsInterface.h
vp2MayaPostEffectsSample.cpp
vp2MayaPostEffectsSample.h
vp2MayaPostEffectsSampleMain.cpp
)

# set linking libraries
set(LIBRARIES
     OpenMaya
     OpenMayaRender
     Foundation
)

# Build plugin
build_plugin()

