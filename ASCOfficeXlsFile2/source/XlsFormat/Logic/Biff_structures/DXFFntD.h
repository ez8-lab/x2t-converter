#pragma once

#include "BiffStructure.h"
#include <Logic/Biff_structures/Stxp.h>
#include <Logic/Biff_structures/BiffString.h>
#include <Logic/Biff_structures/Boolean.h>

namespace XLS
{;

class CFRecord;

class DXFFntD : public BiffStructure
{
	BASE_STRUCTURE_DEFINE_CLASS_NAME(DXFFntD)
public:
	BiffStructurePtr clone();

	//virtual void setXMLAttributes(MSXML2::IXMLDOMElementPtr xml_tag);
	//virtual void getXMLAttributes(MSXML2::IXMLDOMElementPtr xml_tag);
	virtual void load(CFRecord& record);
	virtual void store(CFRecord& record);

private:
	XLUnicodeStringNoCch stFontName;
	Stxp stxp;
	long icvFore;
	Ts tsNinch;
	Boolean<unsigned int> fSssNinch;
	Boolean<unsigned int> fUlsNinch;
	Boolean<unsigned int> fBlsNinch;
	long ich;
	long cch;
	unsigned short iFnt;

};

} // namespace XLS

