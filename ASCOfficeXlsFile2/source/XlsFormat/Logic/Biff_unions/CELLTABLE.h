#pragma once

#include <Logic/CompositeObject.h>

namespace XLS
{;

class CellRef;

// Logical representation of CELLTABLE union of records 
class CELLTABLE: public CompositeObject
{
	BASE_OBJECT_DEFINE_CLASS_NAME(CELLTABLE)
public:
	CELLTABLE(std::vector<CellRef>& shared_formulas_locations_ref);
	~CELLTABLE();

	BaseObjectPtr clone();

	virtual const bool loadContent(BinProcessor& proc);

private:
	std::vector<CellRef>& shared_formulas_locations_ref_;
};

} // namespace XLS
