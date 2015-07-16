#pragma once

#include <Logic/CompositeObject.h>

namespace XLS
{;


// Logical representation of WorksheetSubstream union of records 
class WorksheetSubstream;
typedef boost::shared_ptr<WorksheetSubstream>	WorksheetSubstreamPtr;

class WorksheetSubstream: public CompositeObject
{
	BASE_OBJECT_DEFINE_CLASS_NAME(WorksheetSubstream)
public:
	WorksheetSubstream(const size_t ws_index);
	~WorksheetSubstream();

	BaseObjectPtr clone();

	virtual const bool loadContent(BinProcessor& proc);
	//virtual void toXML(MSXML2::IXMLDOMElementPtr own_tag);
	static const ElementType	type = typeWorksheetSubstream;

private:
	size_t ws_index_;
};

} // namespace XLS

