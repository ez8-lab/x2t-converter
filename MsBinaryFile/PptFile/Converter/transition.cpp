#include "transition.h"

#include "../../../OOXML/PPTXFormat/Logic/Transitions/Transition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/TransitionBase.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/EmptyTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/OrientationTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/EightDirectionTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/OptionalBlackTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/SideDirectionTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/CornerDirectionTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/WheelTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/SplitTransition.h"
#include "../../../OOXML/PPTXFormat/Logic/Transitions/ZoomTransition.h"


using namespace PPT_FORMAT::Converter;


Transition::Transition(const PPT_FORMAT::CSlideShowInfo &SSInfo, CRelsGenerator *pRels) :
    slideShowInfo(SSInfo), oldTransition(SSInfo.m_oTransition),
    pRels(pRels)
{

}

PPTX::Logic::Transition Transition::Convert()
{
    ConvertClick();
    ConvertSpd();
    ConvertDuration();
    ConvertEffect();
    ConvertAudioEffect();

    return std::move(newTransition);
}

void Transition::ConvertClick()
{
    if (slideShowInfo.m_bManulClick == false)
        newTransition.advClick = false;
}

void Transition::ConvertSpd()
{
    newTransition.spd = new PPTX::Limit::TransitionSpeed;
    switch (slideShowInfo.m_oTransition.m_nSpeed)
    {
    case 0: newTransition.spd->set(L"slow"); break;
    case 1: newTransition.spd->set(L"med"); break;
    case 2: // fast
    default:
        newTransition.spd.reset();
    }
}

void Transition::ConvertDuration()
{
    auto duration = slideShowInfo.m_dSlideDuration;
    if (slideShowInfo.m_bAdvClick &&
        duration >= 0 && duration <= 86399000)
    {
        newTransition.advTm = duration;
    }
}

void Transition::ConvertEffect()
{
    switch(oldTransition.m_nEffectType)
    {
    case 1:
    case 17:
    case 18:
    case 19:
    case 22:
    case 23:
    case 27:
    {
        auto pTrBase = new PPTX::Logic::EmptyTransition();
        switch (oldTransition.m_nEffectType)
        {
        case 1:  pTrBase->name = L"random"; break;
        case 17: pTrBase->name = L"diamond"; break;
        case 18: pTrBase->name = L"plus"; break;
        case 19: pTrBase->name = L"wedge"; break;
        case 22: pTrBase->name = L"newsflash"; break;
        case 23: pTrBase->name = L"fade"; break;
        case 27: pTrBase->name = L"circle"; break;
        }
        newTransition.base.base.reset(pTrBase);
    }break;
    case 2:
    case 3:
    case 8:
    case 21:
    {
        auto pTrBase = new PPTX::Logic::OrientationTransition;
        switch (oldTransition.m_nEffectType)
        {
        case 2:  pTrBase->name = L"blinds"; break;
        case 3:  pTrBase->name = L"checker"; break;
        case 8:  pTrBase->name = L"randomBar"; break;
        case 21: pTrBase->name = L"comb"; break;
        }
        pTrBase->dir = new PPTX::Limit::Orient;
        pTrBase->dir->SetBYTECode(oldTransition.m_nEffectDirection);
        newTransition.base.base.reset(pTrBase);
    }break;
    case 0:
    case 5:
    case 6:
    {
        // case was not documented
        if ((UINT)oldTransition.m_nEffectDirection > 1)
            break;

        auto pTrBase = new PPTX::Logic::OptionalBlackTransition;
        if      (oldTransition.m_nEffectType == 0) pTrBase->name	= L"cut";
        else if (oldTransition.m_nEffectType == 5) pTrBase->name	= L"dissolve";
        else if (oldTransition.m_nEffectType == 6) pTrBase->name	= L"fade";

        if (oldTransition.m_nEffectDirection)
            pTrBase->thruBlk = (bool)oldTransition.m_nEffectDirection;

        newTransition.base.base.reset(pTrBase);
    }break;
    case 4:
    case 7:
    {
        auto pTrBase = new PPTX::Logic::EightDirectionTransition;
        if      (oldTransition.m_nEffectType == 4)	pTrBase->name = L"cover";
        else if (oldTransition.m_nEffectType == 7)	pTrBase->name = L"pull";

        pTrBase->dir = new PPTX::Limit::EightDirectionVal;
        switch(oldTransition.m_nEffectDirection)
        {
        case 0:	param_value = L"r"; break;
        case 1:	param_value = L"b"; break;
        case 2:	param_value = L"l"; break;
        case 3:	param_value = L"t"; break;
        case 4:	param_value = L"br"; break;
        case 5:	param_value = L"bl"; break;
        case 6:	param_value = L"tr"; break;
        case 7:	param_value = L"tl"; break;
        }
        pTrBase->dir->set(param_value);
        newTransition.base.base.reset(pTrBase);
    }break;
    case 9:
    {

        auto pTrBase = new PPTX::Logic::CornerDirectionTransition;
        pTrBase->name = L"strips";
        pTrBase->dir = new PPTX::Limit::CornerDirectionVal;
        switch(oldTransition.m_nEffectDirection)
        {
        case 0:	param_value = L"ru"; break;
        case 1:	param_value = L"lu"; break;
        case 2:	param_value = L"rd"; break;
        case 3:	param_value = L"ld"; break;
        }
        pTrBase->dir->set(param_value);
        newTransition.base.base.reset(pTrBase);
    }break;
    case 10:
    case 20:
    {
        auto pTrBase = new PPTX::Logic::SideDirectionTransition;
        if (oldTransition.m_nEffectType == 10)	pTrBase->name = L"wipe";
        if (oldTransition.m_nEffectType == 20)	pTrBase->name = L"push";

        pTrBase->dir = new PPTX::Limit::SideDirectionVal;
        switch(oldTransition.m_nEffectDirection)
        {
        case 0:	param_value = L"l"; break;
        case 1:	param_value = L"u"; break;
        case 2:	param_value = L"r"; break;
        case 3:	param_value = L"d"; break;
        }
        pTrBase->dir->set(param_value);
        newTransition.base.base.reset(pTrBase);
    }break;
    case 11:
    {
        auto pTrBase = new PPTX::Logic::ZoomTransition;
        pTrBase->dir = new PPTX::Limit::InOutDirectionVal;
        pTrBase->dir->SetBYTECode(!oldTransition.m_nEffectDirection);
        newTransition.base.base.reset(pTrBase);
    }break;
    case 13:
    {
        auto pTrBase = new PPTX::Logic::SplitTransition;
        pTrBase->dir = new PPTX::Limit::InOutDirectionVal;
        pTrBase->orient = new PPTX::Limit::Orient;

        switch(oldTransition.m_nEffectDirection)
        {
        case 0:	param_value2 = L"horz";	param_value = L"out";	break;
        case 1:	param_value2 = L"horz";	param_value = L"in";	break;
        case 2:	param_value2 = L"vert";	param_value = L"out";	break;
        case 3:	param_value2 = L"vert";	param_value = L"in";	break;
        }
        pTrBase->dir->set(param_value);
        pTrBase->orient->set(param_value2);

        newTransition.base.base.reset(pTrBase);
    }break;
    case 26:
    {
        auto pTrBase = new PPTX::Logic::WheelTransition;
        pTrBase->spokes = oldTransition.m_nEffectDirection;
        newTransition.base.base.reset(pTrBase);
    }break;
    default:
        break;
    }
}

void Transition::ConvertAudioEffect()
{
    if (HasAudio() == false) return;

    InitSound();
    WriteAudioRId();
    WriteSoundName();
}

std::wstring Transition::GetAudioRId()
{
    bool bExternal = false;
    return pRels->WriteAudio(oldTransition.m_oAudio.m_strAudioFileName, bExternal);
}

void Transition::WriteAudioRId()
{
    newTransition.sndAc->stSnd->embed = GetAudioRId();
}

void Transition::InitSound()
{
    newTransition.sndAc = new PPTX::Logic::SndAc;
    newTransition.sndAc->stSnd = new PPTX::Logic::StSnd;
}

bool Transition::HasAudio() const
{
    return oldTransition.m_bAudioPresent == true && pRels != nullptr;
}

void Transition::WriteSoundName()
{
    if (!oldTransition.m_oAudio.m_sImageName.empty())
        newTransition.sndAc->stSnd->name = XmlUtils::EncodeXmlString(oldTransition.m_oAudio.m_sImageName);
}
