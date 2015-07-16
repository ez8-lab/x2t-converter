#include "precompiled_xls.h"
#include "Pie.h"

namespace XLS
{;

Pie::Pie()
{
}


Pie::~Pie()
{
}


BaseObjectPtr Pie::clone()
{
	return BaseObjectPtr(new Pie(*this));
}


void Pie::writeFields(CFRecord& record)
{
	unsigned short flags = 0;
	SETBIT(flags, 0, fHasShadow);
	SETBIT(flags, 1, fShowLdrLines);
	record >> anStart >> pcDonut >> flags;
}


void Pie::readFields(CFRecord& record)
{
	unsigned short flags;
	record >> anStart >> pcDonut >> flags;
	fHasShadow = GETBIT(flags, 0);
	fShowLdrLines = GETBIT(flags, 1);
}

} // namespace XLS

