#include "OverrideClearColorInterface.h"

namespace amagly
{
    OverrideClearColorInterface::OverrideClearColorInterface(unsigned int clearMask)
        : mClearMask(clearMask), mOverrideClearColor(false), mClearColor(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    OverrideClearColorInterface::~OverrideClearColorInterface()
    {
    }
}