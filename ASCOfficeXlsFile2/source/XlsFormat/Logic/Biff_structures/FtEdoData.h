#pragma once

#include "BiffStructure.h"
#include "Boolean.h"

namespace XLS
{;

class CFRecord;

class FtEdoData : public BiffStructure
{
	BASE_STRUCTURE_DEFINE_CLASS_NAME(FtEdoData)
public:
	BiffStructurePtr clone();

	//virtual void setXMLAttributes(MSXML2::IXMLDOMElementPtr own_tag);
	//virtual void getXMLAttributes(MSXML2::IXMLDOMElementPtr own_tag);
	virtual void load(CFRecord& record);
	virtual void store(CFRecord& record);

private:
	unsigned short ivtEdit;
	Boolean<unsigned short> fMultiLine;
	unsigned short fVScroll;
	unsigned short id;
};

} // namespace XLS

