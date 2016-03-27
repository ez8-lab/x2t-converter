
#include "PtgAreaN.h"
#include "CellRangeRef.h"
#include <Binary/CFRecord.h>

namespace XLS
{


PtgAreaN::PtgAreaN(const CellRef cell_base_ref_init)
:	cell_base_ref(cell_base_ref_init)
{
}


PtgAreaN::PtgAreaN(const std::wstring& word, const PtgDataType data_type, const CellRef cell_base_ref_init)
:	OperandPtg(fixed_id | (static_cast<unsigned char>(data_type) << 5)),
	area(word),
	cell_base_ref(cell_base_ref_init)
{
	area -= cell_base_ref;
}


BiffStructurePtr PtgAreaN::clone()
{
	return BiffStructurePtr(new PtgAreaN(*this));
}

void PtgAreaN::storeFields(CFRecord& record)
{
	record << area;
}


void PtgAreaN::loadFields(CFRecord& record)
{
	if (record.getGlobalWorkbookInfo()->Version < 0x600)
	{
		unsigned char	colFirst, colLast;
		_UINT16			rwFirst, rwLast;

		record >> rwFirst >> rwLast >> colFirst >> colLast;

		area.rowFirstRelative		= rwFirst & 0x8000;
		area.columnFirstRelative	= rwFirst & 0x4000;
		
		area.columnFirst			= colFirst;
		area.rowFirst				= rwFirst & 0x3FFF;

		area.rowLastRelative	= rwLast & 0x8000;
		area.columnLastRelative	= rwLast & 0x4000;
		
		area.columnLast			= colLast;
		area.rowLast			= rwLast & 0x3FFF;
	}
	else
		record >> area;
}


void PtgAreaN::assemble(AssemblerStack& ptg_stack, PtgQueue& extra_data, bool full_ref)
{
	ptg_stack.push((area + cell_base_ref).toString());
}


} // namespace XLS

