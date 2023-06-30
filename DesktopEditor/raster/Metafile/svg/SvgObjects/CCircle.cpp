#include "CCircle.h"

#include "CContainer.h"
#include "CStyle.h"

namespace SVG
{
	CCircle::CCircle(XmlUtils::CXmlNode& oNode, CRenderedObject* pParent)
		: CRenderedObject(oNode, pParent)
	{
		m_oCx.SetValue(oNode.GetAttribute(L"cx"));
		m_oCy.SetValue(oNode.GetAttribute(L"cy"));
		m_oR .SetValue(oNode.GetAttribute(L"r"));
	}

	void CCircle::SetData(const std::map<std::wstring, std::wstring> &mAttributes, unsigned short ushLevel, bool bHardMode)
	{
		SetTransform(mAttributes, ushLevel, bHardMode);
		SetStroke(mAttributes, ushLevel, bHardMode);
		SetFill(mAttributes, ushLevel, bHardMode);
		SetClip(mAttributes, ushLevel, bHardMode);
		SetMask(mAttributes, ushLevel, bHardMode);
		SetDisplay(mAttributes, ushLevel, bHardMode);
	}

	bool CCircle::Draw(IRenderer *pRenderer, const CSvgFile *pFile, CommandeMode oMode, const TSvgStyles *pOtherStyles) const
	{
		if (NULL == pRenderer || !m_oStyles.m_bDisplay)
			return false;

		TBounds oBounds = (NULL != m_pParent) ? m_pParent->GetBounds() : TBounds{0., 0., 0., 0.};

		double dX = m_oCx.ToDouble(NSCSS::Pixel, oBounds.m_dRight  - oBounds.m_dLeft);
		double dY = m_oCy.ToDouble(NSCSS::Pixel, oBounds.m_dBottom - oBounds.m_dTop);
		double dR = m_oR .ToDouble(NSCSS::Pixel);

		StartPath(pRenderer, pFile, oMode);

		pRenderer->PathCommandMoveTo(dX + dR, dY);
		pRenderer->PathCommandArcTo(dX - dR, dY - dR, dR * 2.0, dR * 2.0, 0, 360);

		EndPath(pRenderer, pFile, oMode, pOtherStyles);

		return true;
	}

	void CCircle::ApplyStyle(IRenderer *pRenderer, const TSvgStyles *pStyles, const CSvgFile *pFile, int &nTypePath, Aggplus::CMatrix &oOldMatrix) const
	{
		Apply(pRenderer, &pStyles->m_oTransform, oOldMatrix);

		if (Apply(pRenderer, &pStyles->m_oStroke))
			nTypePath += c_nStroke;

		if (Apply(pRenderer, &pStyles->m_oFill, pFile, true))
			nTypePath += c_nWindingFillMode;
	}

	TBounds CCircle::GetBounds() const
	{
		TBounds oBounds;

		oBounds.m_dLeft   = (m_oCx - m_oR).ToDouble(NSCSS::Pixel);
		oBounds.m_dTop    = (m_oCy - m_oR).ToDouble(NSCSS::Pixel);
		oBounds.m_dRight  = (m_oCx + m_oR).ToDouble(NSCSS::Pixel);
		oBounds.m_dBottom = (m_oCy + m_oR).ToDouble(NSCSS::Pixel);

		return oBounds;
	}

}
