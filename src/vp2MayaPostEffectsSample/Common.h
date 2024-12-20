#pragma once

namespace amagly
{
    // Array size
    #define ARRAYSIZE(a) std::size(a)

    // Maya Output
    #define MDisplayInfo(...)		{char _buf[512];_snprintf(_buf,ARRAYSIZE(_buf)-1,__VA_ARGS__);_buf[ARRAYSIZE(_buf)-1]='\0';MGlobal::displayInfo(MString(_buf));}
    #define MDisplayWarning(...)	{char _buf[512];_snprintf(_buf,ARRAYSIZE(_buf)-1,__VA_ARGS__);_buf[ARRAYSIZE(_buf)-1]='\0';MGlobal::displayWarning(MString(_buf));}
    #define MDisplayError(...)		{char _buf[512];_snprintf(_buf,ARRAYSIZE(_buf)-1,__VA_ARGS__);_buf[ARRAYSIZE(_buf)-1]='\0';MGlobal::displayError(MString(_buf));}

    #define AMA_DSP_INFO "[AMGPostEffect:Info]"
    #define AMA_DSP_WARN "[AMGPostEffect:Warning]"
    #define AMA_DSP_EROR "[AMGPostEffect:Error]"
} // namespace amagly
