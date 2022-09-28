#include "Page.h"
#include "../resources/Constants.h"
#include "../resources/utils.h"
#include <memory>

namespace NSDocxRenderer
{
    CPage::CPage(NSFonts::IApplicationFonts* pFonts) : m_oFontManagerLight(pFonts)
    {
    }

    void CPage::Init(NSStructures::CFont* pFont, NSStructures::CPen* pPen, NSStructures::CBrush* pBrush,
                     NSStructures::CShadow* pShadow, NSStructures::CEdgeText* pEdge, Aggplus::CMatrix* pMatrix,
                     Aggplus::CGraphicsPathSimpleConverter* pSimple, CStyleManager* pStyleManager, CFontManager *pFontManager)
    {
        m_pFont     = pFont;
        m_pPen      = pPen;
        m_pBrush    = pBrush;
        m_pShadow   = pShadow;
        m_pEdgeText = pEdge;

        m_pTransform               = pMatrix;
        m_pSimpleGraphicsConverter = pSimple;

        m_pStyleManager = pStyleManager;
        m_pFontManager = pFontManager;

        m_pCurrentLine = nullptr;

        CShape::ResetRelativeHeight();
    }

    void CPage::Clear()
    {
        ClearTextData();
        ClearTextLines();
        ClearParagraphs();
        ClearShapes();
        ClearImages();

        m_pCurrentLine = nullptr;
    }

    void CPage::ClearImages()
    {
        m_arImages.clear();
    }

    void CPage::ClearTextData()
    {
        m_arDiacriticalSymbol.clear();
    }

    void CPage::ClearTextLines()
    {
        m_arTextLine.clear();
    }

    void CPage::ClearShapes()
    {
        m_arShapes.clear();
    }

    void CPage::ClearParagraphs()
    {
        m_arParagraphs.clear();
    }

    CPage::~CPage()
    {
        Clear();
    }

    void CPage::DeleteTextClipPage()
    {
        if (m_bIsDeleteTextClipPage)
        {
            // удалим все линии, которые выходят за границы страницы
            for (size_t i = 0; i < m_arTextLine.size(); ++i)
            {
                auto pLine = m_arTextLine[i];

                if (pLine->m_dTop >= m_dHeight || pLine->m_dBaselinePos <= 0)
                {
                    pLine->m_bIsNotNecessaryToUse = true;
                }
            }
        }
    }

    // image commands
    void CPage::WriteImage(const std::shared_ptr<CImageInfo> pInfo, double& fX, double& fY, double& fWidth, double& fHeight)
    {
        auto pImage = new CShape(pInfo, L"");
        pImage->m_eType = CShape::eShapeType::stPicture;

        double dRotation = m_pTransform->z_Rotation();

        if (fabs(dRotation) < 5.0)
        {
            double x1 = fX;
            double y1 = fY;
            double x2 = fX + fWidth;
            double y2 = fY + fHeight;

            m_pTransform->TransformPoint(x1, y1);
            m_pTransform->TransformPoint(x2, y2);

            if (x1 <= x2)
            {
                pImage->m_dLeft     = x1;
                pImage->m_dWidth    = x2 - x1;
            }
            else
            {
                pImage->m_dLeft     = x2;
                pImage->m_dWidth    = x1 - x2;
            }

            if (y1 <= y2)
            {
                pImage->m_dTop      = y1;
                pImage->m_dHeight   = y2 - y1;
            }
            else
            {
                pImage->m_dTop      = y2;
                pImage->m_dHeight   = y1 - y2;
            }

            pImage->m_dRotate = 0.0;
        }
        else
        {
            double x1 = fX;
            double y1 = fY;
            double x2 = fX + fWidth;
            double y2 = fY + fHeight;

            Aggplus::CMatrix oTemp  = *m_pTransform;

            double dCx = (x1 + x2) / 2;
            double dCy = (y1 + y2) / 2;
            m_pTransform->TransformPoint(dCx, dCy);
            oTemp.RotateAt(-dRotation, dCx, dCy, Aggplus::MatrixOrderAppend);

            oTemp.TransformPoint(x1, y1);
            oTemp.TransformPoint(x2, y2);

            if (x1 <= x2)
            {
                pImage->m_dLeft     = x1;
                pImage->m_dWidth    = x2 - x1;
            }
            else
            {
                pImage->m_dLeft     = x2;
                pImage->m_dWidth    = x1 - x2;
            }

            if (y1 <= y2)
            {
                pImage->m_dTop      = y1;
                pImage->m_dHeight   = y2 - y1;
            }
            else
            {
                pImage->m_dTop      = y2;
                pImage->m_dHeight   = y1 - y2;
            }

            pImage->m_dRotate = dRotation;
        }

        pImage->m_dBaselinePos = pImage->m_dTop + pImage->m_dHeight;
        pImage->m_dRight = pImage->m_dLeft + pImage->m_dWidth;

        m_arImages.push_back(pImage);
    }

    // path commands
    void CPage::MoveTo(double& dX, double& dY)
    {
        m_pTransform->TransformPoint(dX, dY);
        m_oVector.MoveTo(dX, dY);
    }

    void CPage::LineTo(double& dX, double& dY)
    {
        m_pTransform->TransformPoint(dX, dY);
        m_oVector.LineTo(dX, dY);
    }

    void CPage::CurveTo(double& x1, double& y1, double& x2, double& y2, double& x3, double& y3)
    {
        m_pTransform->TransformPoint(x1, y1);
        m_pTransform->TransformPoint(x2, y2);
        m_pTransform->TransformPoint(x3, y3);

        m_oVector.CurveTo(x1, y1, x2, y2, x3, y3);
    }

    void CPage::Start()
    {
    }

    void CPage::End()
    {
        m_oVector.End();
    }

    void CPage::Close()
    {
        m_oVector.Close();
    }

    void CPage::DrawPath(LONG lType, const std::shared_ptr<CImageInfo> pInfo)
    {
        if ((m_oVector.m_dLeft <= m_oVector.m_dRight) && (m_oVector.m_dTop <= m_oVector.m_dBottom))
        {
            if (!m_arShapes.empty())
            {
                auto pLastShape = m_arShapes.back();

                if (pLastShape->m_dLeft == m_oVector.m_dLeft &&
                    pLastShape->m_dTop == m_oVector.m_dTop &&
                    pLastShape->m_dWidth == m_oVector.m_dRight - m_oVector.m_dLeft &&
                    pLastShape->m_dHeight == m_oVector.m_dBottom - m_oVector.m_dTop)
                {
                    if (0x00 != (lType & 0x01))
                    {
                        pLastShape->m_bIsNoStroke = false;
                        pLastShape->m_oPen = *m_pPen;
                    }
                    if (0x00 != (lType >> 8))
                    {
                        pLastShape->m_bIsNoFill = false;
                        pLastShape->m_oBrush = *m_pBrush;
                    }
                    return;
                }
            }

            auto pShape = new CShape();

            if (pInfo)
            {
                pShape->m_pImageInfo = pInfo;
                pShape->m_eType = CShape::eShapeType::stVectorTexture;
            }
            else
            {
                pShape->m_eType = CShape::eShapeType::stVectorGraphics;
            }

            if (0x00 != (lType & 0x01))
            {
                pShape->m_bIsNoStroke = false;
                pShape->m_oPen = *m_pPen;
            }
            if (0x00 != (lType >> 8))
            {
                pShape->m_bIsNoFill = false;
                pShape->m_oBrush = *m_pBrush;
            }

            if (pShape->m_bIsNoStroke)
            {
                if ((fabs(m_oVector.m_dLeft - m_oVector.m_dRight) < 0.3) || (fabs(m_oVector.m_dTop - m_oVector.m_dBottom) < 0.3))
                {
                    pShape->m_oPen.Color = m_pBrush->Color1;
                    pShape->m_oPen.Alpha = m_pBrush->Alpha1;
                }
            }

            pShape->GetDataFromVector(m_oVector);

            m_arShapes.push_back(pShape);
        }
    }

    void CPage::CollectTextData(const PUINT pUnicodes, const PUINT pGids, const UINT& nCount,
                                const double& fX, const double& fY, const double& fWidth, const double& fHeight,
                                const double& fBaseLineOffset, const bool& bIsPDFAnalyzer)
    {
        if (pUnicodes != nullptr && nCount == 1 && IsSpaceUtf32(*pUnicodes))
        {
            //note пробелы не нужны, добавляются при анализе
            return;
        }

        double dTextX = fX;
        double dTextY = fY;
        double dTextR = fX + fWidth;
        double dTextB = fY + fHeight;

        m_pTransform->TransformPoint(dTextX, dTextY);
        m_pTransform->TransformPoint(dTextR, dTextB);

        double dTextW = dTextR - dTextX;
        double dTextH = dTextB - dTextY;

        NSStringUtils::CStringUTF32 oText((uint32_t*)pUnicodes, nCount);

        if ((pUnicodes != nullptr) && (pGids != nullptr))
        {
            for (unsigned int i = 0; i < nCount; ++i)
            {
                if ( !IsUnicodeSymbol( pUnicodes[i] ) )
                {
                    oText[i] = ' ';
                }
            }
        }

        bool bIsPath = ((nullptr == pGids) && !bIsPDFAnalyzer) ? false : true;

        m_pFontManager->LoadFont(0, !bIsPath);

        if (!bIsPath)
            m_pFontManager->GenerateFontName2(oText);

        if (fabs(dTextW) < 0.01 || (dTextW > 10))
        {
            double _x = 0;
            double _y = 0;
            double _w = 0;
            double _h = 0;

            if (nullptr != pGids)
            {
                m_pFontManager->SetStringGid(1);
                m_pFontManager->MeasureStringGids(pGids, nCount, dTextX, dTextY, _x, _y, _w, _h, CFontManager::mtPosition);
            }
            else
            {
                // такого быть не должно (только из xps)
                m_pFontManager->SetStringGid(0);
                m_pFontManager->MeasureStringGids(pUnicodes, nCount, dTextX, dTextY, _x, _y, _w, _h, CFontManager::mtPosition);
            }

            dTextW = _w;
        }

        double dBaseLinePos = dTextY + fBaseLineOffset;
        dTextH = m_pFontManager->GetFontHeight();

        auto pCont = new CContText(&m_oFontManagerLight, m_pStyleManager);

        pCont->m_dLeft = dTextX;
        pCont->m_dBaselinePos = dBaseLinePos;

        pCont->m_dTop       = dBaseLinePos - dTextH - m_pFontManager->m_oFont.m_dBaselineOffset;
        pCont->m_dWidth     = dTextW;
        pCont->m_dHeight    = dTextH;
        pCont->m_dRight     = dTextX + dTextW;

        pCont->m_oText = oText;

        //Первичное заполнение стилей
        m_pStyleManager->m_pCurrentStyle->m_oFont = m_pFontManager->m_oFont.m_oFont;
        m_pStyleManager->m_pCurrentStyle->m_oBrush = *m_pBrush;

        if (!bIsPath)
        {
            m_pStyleManager->m_pCurrentStyle->m_strPickFontName = m_pFontManager->m_strCurrentPickFont;
            m_pStyleManager->m_pCurrentStyle->m_lPickFontStyle  = m_pFontManager->m_lCurrentPictFontStyle;
        }

        pCont->m_pFontStyle = m_pStyleManager->GetStyle();

        pCont->m_dSpaceWidthMM = m_pFontManager->m_dSpaceWidthMM;

        if (nCount == 1 && IsDiacriticalMark(*pUnicodes))
        {
            m_arDiacriticalSymbol.push_back(pCont);
        }
        else
        {
            AddContToTextLine(pCont);
        }
    }

    void CPage::AddContToTextLine(CContText *pCont)
    {
        if (nullptr == m_pCurrentLine)
        {
            auto pLine = new CTextLine();
            m_pCurrentLine = pLine;
            m_pCurrentLine->AddContent(pCont);
            m_arTextLine.push_back(pLine);
            return;
        }

        if (fabs(m_pCurrentLine->m_dBaselinePos - pCont->m_dBaselinePos) <= c_dTHE_SAME_STRING_Y_PRECISION_MM)
        {
            m_pCurrentLine->AddContent(pCont);
            return;
        }

        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            if (fabs(m_arTextLine[i]->m_dBaselinePos - pCont->m_dBaselinePos) <= c_dTHE_SAME_STRING_Y_PRECISION_MM)
            {
                m_pCurrentLine = m_arTextLine[i];
                m_pCurrentLine->AddContent(pCont);
                return;
            }
        }

        auto pLine = new CTextLine();
        m_pCurrentLine = pLine;
        m_pCurrentLine->AddContent(pCont);
        m_arTextLine.push_back(pLine);
    }

    void CPage::ProcessingAndRecordingOfPageData(NSStringUtils::CStringBuilder& oWriter, LONG lPagesCount, LONG lNumberPages)
    {
        AnalyzeCollectedShapes();
        AnalyzeCollectedTextLines();
        BuildByType();
        ToXml(oWriter);
        WriteSectionToFile(lPagesCount >= lNumberPages - 1, oWriter);
    }

    void CPage::AnalyzeCollectedShapes()
    {
        DetermineLinesType();
    }

    void CPage::DetermineLinesType()
    {
        for (size_t i = 0; i < m_arShapes.size(); ++i)
        {
            auto pCurrShape = m_arShapes[i];

            if (pCurrShape->m_bIsNotNecessaryToUse ||
                pCurrShape->m_dHeight > c_dMAX_LINE_HEIGHT_MM || //рассматриваем только тонкие объекты
                (pCurrShape->m_eGraphicsType != eGraphicsType::gtRectangle &&
                 pCurrShape->m_eGraphicsType != eGraphicsType::gtCurve))
            {
                continue;
            }

            //Нужно собрать всю графику, которая находится на одной линии
            std::vector<CShape*> arCurrShapes;
            arCurrShapes.push_back(m_arShapes[i]);

            for (size_t j = i+1; j < m_arShapes.size(); ++j)
            {
                auto pNextShape = m_arShapes[j];
                if (pNextShape->m_bIsNotNecessaryToUse || pCurrShape->AreObjectsNoCrossing(pNextShape))
                {
                    continue;
                }
                bool bIf1 = pCurrShape->IsCorrelated(pNextShape);
                //note довольно странное поведение - в зависимости от толщины линии информация о графике записывается в разные структуры
                bool bIf2 = pCurrShape->m_oBrush.IsEqual(&pNextShape->m_oBrush);
                bool bIf3 = pCurrShape->m_oPen.IsEqual(&pNextShape->m_oPen);
                //линия должна быть одного размера по высоте
                bool bIf4 = fabs(pCurrShape->m_dHeight - pNextShape->m_dHeight) < c_dGRAPHICS_ERROR_IN_LINES_MM;
                //все должно быть на одной линии
                bool bIf5 = fabs(pCurrShape->m_dTop - pNextShape->m_dTop) < c_dGRAPHICS_ERROR_IN_LINES_MM * 5;

                if (bIf1 && (bIf2 || bIf3) && bIf4 && bIf5) //все должно быть на одной линии
                {
                    arCurrShapes.push_back(pNextShape);
                }
            }

            //Отсортируем собранный массив по x
            size_t nCurrShapesCount = arCurrShapes.size();
            if (nCurrShapesCount > 1)
            {
                SortElements(arCurrShapes);
                pCurrShape = arCurrShapes[0];

                //сравнение
                for (size_t k = 1; k < nCurrShapesCount; ++k)
                {
                    auto pNextShape = arCurrShapes[k];

                    //note логика работатет только если arCurrShapes отсортирован по m_dLeft
                    pCurrShape->DetermineLineType(pNextShape, k == nCurrShapesCount - 1);

                    if (pCurrShape->m_bIsNotNecessaryToUse)
                    {
                        pCurrShape = pNextShape;
                        k++;
                    }
                }
            }
            else if (nCurrShapesCount == 1)
            {
                arCurrShapes[0]->DetermineLineType();
            }

            arCurrShapes.clear();
        }
    }

    void CPage::AnalyzeCollectedTextLines()
    {
        SortElements(m_arTextLine);
        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            m_arTextLine[i]->SortConts();
        }

        AnalyzeCollectedConts();

        DetermineStrikeoutsUnderlinesHighlights();

        AddDiacriticalSymbols();

        MergeLinesByVertAlignType();

        BuildLines();

        DetermineDominantGraphics();

        DeleteTextClipPage();
    }

    void CPage::AnalyzeCollectedConts()
    {
        for (size_t uCurrLineIndex = 0; uCurrLineIndex < m_arTextLine.size(); ++uCurrLineIndex)
        {
            auto pCurrLine = m_arTextLine[uCurrLineIndex];

            if (pCurrLine->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            for (size_t uCurrContIndex = 0; uCurrContIndex < pCurrLine->m_arConts.size(); ++uCurrContIndex)
            {
                auto pCurrCont = pCurrLine->m_arConts[uCurrContIndex];

                if (pCurrCont->m_bIsNotNecessaryToUse)
                {
                    continue;
                }

                for (size_t uNextLineIndex = uCurrContIndex >= pCurrLine->m_arConts.size() - 1 ?
                     uCurrLineIndex + 1 : uCurrLineIndex; uNextLineIndex < m_arTextLine.size(); ++uNextLineIndex)
                {
                    auto pNextLine = m_arTextLine[uNextLineIndex];

                    if (pNextLine->m_bIsNotNecessaryToUse || pCurrLine->AreObjectsNoCrossing(pNextLine))
                    {
                        continue;
                    }

                    for (size_t uNextContIndex = uNextLineIndex != uCurrLineIndex ? 0 : uCurrContIndex + 1;
                         uNextContIndex < pNextLine->m_arConts.size(); ++uNextContIndex)
                    {
                        auto pNextCont = pNextLine->m_arConts[uNextContIndex];

                        if (pNextCont->m_bIsNotNecessaryToUse)
                        {
                            continue;
                        }

                        eVerticalCrossingType eVType = pCurrCont->GetVerticalCrossingType(pNextCont);
                        eHorizontalCrossingType eHType = pCurrCont->GetHorizontalCrossingType(pNextCont);

                        if (pCurrCont->IsThereAreFontEffects(pNextCont, eVType, eHType))
                        {
                            pCurrLine->CheckLineToNecessaryToUse();
                            break;
                        }

                        if (pCurrCont->IsVertAlignTypeBetweenConts(pNextCont, eVType, eHType))
                        {
                            pCurrLine->SetVertAlignType(pCurrCont->m_eVertAlignType);
                            pNextLine->SetVertAlignType(pNextCont->m_eVertAlignType);
                            if ((pCurrLine->m_eVertAlignType == eVertAlignType::vatSuperscript &&
                                pNextLine->m_eVertAlignType == eVertAlignType::vatBase) ||
                                (pCurrLine->m_eVertAlignType == eVertAlignType::vatBase &&
                                 pNextLine->m_eVertAlignType == eVertAlignType::vatSubscript))
                            {
                                pCurrLine->m_pLine = pNextLine;
                            }
                            break;
                        }

                        if (pCurrCont->IsDuplicate(pNextCont, eVType))
                        {
                            break;
                        }
                    }

                    pNextLine->CheckLineToNecessaryToUse();
                }
            }
        }
    }

    void CPage::DetermineStrikeoutsUnderlinesHighlights()
    {
        for (size_t i = 0; i < m_arShapes.size(); ++i)
        {
            auto pShape = m_arShapes[i];

            if (pShape->m_eGraphicsType == eGraphicsType::gtNoGraphics ||
                pShape->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            for (size_t j = 0; j < m_arTextLine.size(); ++j)
            {
                auto pCurrLine = m_arTextLine[j];

                if (pCurrLine->m_bIsNotNecessaryToUse ||
                    (pCurrLine->AreObjectsNoCrossing(pShape) &&
                     (pCurrLine->m_dTop > pShape->m_dBaselinePos ||
                      pCurrLine->m_dBaselinePos + pCurrLine->m_dHeight < pShape->m_dTop)))
                {
                    continue;
                }

                for (size_t k = 0; k < pCurrLine->m_arConts.size(); ++k)
                {
                    auto pCurrCont = pCurrLine->m_arConts[k];

                    if (pCurrCont->m_bIsNotNecessaryToUse)
                    {
                        continue;
                    }

                    eVerticalCrossingType eVType = pCurrCont->GetVerticalCrossingType(pShape);
                    eHorizontalCrossingType eHType = pCurrCont->GetHorizontalCrossingType(pShape);

                    bool bIf1 = pShape->m_eGraphicsType != eGraphicsType::gtComplicatedFigure;
                    bool bIf2 = IsLineCrossingText(pShape, pCurrCont, eHType);
                    bool bIf3 = IsLineBelowText(pShape, pCurrCont, eHType);
                    bool bIf4 = IsItHighlightingBackground(pShape, pCurrCont, eHType);

                    if (bIf1 && (bIf2 || bIf3 || bIf4))
                    {
                        pShape->m_bIsNotNecessaryToUse = true;
                    }

                    if (!bIf1)
                    {
                        bool bIf1 = pCurrCont->m_pFontStyle->m_oBrush.Color1 == c_iGreyColor;
                        bool bIf2 = pCurrCont->m_bIsShadowPresent && pCurrCont->m_bIsOutlinePresent;
                        bool bIf3 = eVType == eVerticalCrossingType::vctCurrentOutsideNext;
                        bool bIf4 = eHType == eHorizontalCrossingType::hctCurrentOutsideNext;
                        bool bIf5 = eHType == eHorizontalCrossingType::hctCurrentRightOfNext;

                        if ((bIf1 || bIf2) && bIf3 && (bIf4 || bIf5))
                        {
                            if (!bIf2)
                            {
                                m_pStyleManager->m_pCurrentStyle->CopyFormat(*pCurrCont->m_pFontStyle);
                                m_pStyleManager->m_pCurrentStyle->m_oBrush.Color1 = pShape->m_oPen.Color;
                                pCurrCont->m_pFontStyle = m_pStyleManager->GetStyle();

                                pCurrCont->m_bIsShadowPresent = true;
                                pCurrCont->m_bIsOutlinePresent = true;
                            }

                            pShape->m_bIsNotNecessaryToUse = true;
                        }
                    }
                }
            }
        }
    }

    bool CPage::IsLineCrossingText(const CShape *pShape, CContText *pCont, const eHorizontalCrossingType& eHType)
    {
        double dTopBorder = pCont->m_dTop + pCont->m_dHeight/3; //note Height - это максимально возможный размер символа. Больше реального размера.

        bool bIf1 = pShape->m_eGraphicsType == eGraphicsType::gtRectangle &&
                pShape->m_eLineType != eLineType::ltUnknown;
        //Условие пересечения по вертикали
        bool bIf2 = pShape->m_dTop > dTopBorder && pShape->m_dBaselinePos < pCont->m_dBaselinePos;
        //Условие пересечения по горизонтали
        bool bIf3 = eHType != eHorizontalCrossingType::hctUnknown &&
                    eHType != eHorizontalCrossingType::hctCurrentLeftOfNext &&
                    eHType != eHorizontalCrossingType::hctNoCrossingCurrentLeftOfNext &&
                    eHType != eHorizontalCrossingType::hctNoCrossingCurrentRightOfNext;
        //Условие для размеров по высоте
        bool bIf4 = pShape->m_dHeight < pCont->m_dHeight &&
                pCont->m_dHeight - pShape->m_dHeight > c_dERROR_FOR_TEXT_WITH_GRAPHICS_MM;

        if (bIf1 && bIf2 && bIf3 && bIf4)
        {
            pCont->m_bIsStrikeoutPresent = true;;
            if (pShape->m_eLineType == eLineType::ltDouble)
            {
                pCont->m_bIsDoubleStrikeout = true;
            }
            return true;
        }

        return false;
    }

    bool CPage::IsLineBelowText(const CShape *pShape, CContText *pCont, const eHorizontalCrossingType& eHType)
    {
        bool bIf1 = (pShape->m_eGraphicsType == eGraphicsType::gtRectangle ||
                     pShape->m_eGraphicsType == eGraphicsType::gtCurve) &&
                pShape->m_eLineType != eLineType::ltUnknown;
        //Условие по вертикали
        bool bIf2 = fabs(pShape->m_dTop - pCont->m_dBaselinePos) < pCont->m_dHeight * 0.15;
        //Условие пересечения по горизонтали
        bool bIf3 = eHType != eHorizontalCrossingType::hctUnknown &&
                    eHType != eHorizontalCrossingType::hctCurrentLeftOfNext &&
                    eHType != eHorizontalCrossingType::hctNoCrossingCurrentLeftOfNext &&
                    eHType != eHorizontalCrossingType::hctNoCrossingCurrentRightOfNext;
        //Условие для размеров по высоте
        bool bIf4 = pShape->m_dHeight < pCont->m_dHeight &&
                pCont->m_dHeight - pShape->m_dHeight > c_dERROR_FOR_TEXT_WITH_GRAPHICS_MM;

        if (bIf1 && bIf2 && bIf3 && bIf4)
        {
            pCont->m_bIsUnderlinePresent = true;;
            pCont->m_eUnderlineType  = pShape->m_eLineType;
            pCont->m_lUnderlineColor = pShape->m_dHeight > 0.3 ? pShape->m_oBrush.Color1 : pShape->m_oPen.Color;
            return true;
        }

        return false;
    }

    bool CPage::IsItHighlightingBackground(CShape *pShape, CContText* pCont, const eHorizontalCrossingType& eHType)
    {
        double dSomeBaseLine1 = pCont->m_dBaselinePos - pCont->m_dHeight * 0.75;
        double dSomeBaseLine2 = pCont->m_dBaselinePos - pCont->m_dHeight * 0.5;
        double dSomeBaseLine3 = pCont->m_dBaselinePos - pCont->m_dHeight * 0.25;

        bool bIf1 = pShape->m_eGraphicsType == eGraphicsType::gtRectangle;
        //Условие пересечения по вертикали
        bool bIf2 = (dSomeBaseLine1 > pShape->m_dTop && dSomeBaseLine1 < pShape->m_dBaselinePos &&
                     dSomeBaseLine2 > pShape->m_dTop && dSomeBaseLine2 < pShape->m_dBaselinePos &&
                     dSomeBaseLine3 > pShape->m_dTop && dSomeBaseLine3 < pShape->m_dBaselinePos);
        //Условие пересечения по горизонтали
        bool bIf3 = eHType != eHorizontalCrossingType::hctUnknown &&
                    eHType != eHorizontalCrossingType::hctCurrentLeftOfNext &&
                    eHType != eHorizontalCrossingType::hctNoCrossingCurrentLeftOfNext &&
                    eHType != eHorizontalCrossingType::hctNoCrossingCurrentRightOfNext;
        //Цвета должны быть разными
        bool bIf4 = pCont->m_pFontStyle->m_oBrush.Color1 != pShape->m_oBrush.Color1;
        bool bIf5 = pShape->m_oBrush.Color1 == c_iBlackColor && pShape->m_oPen.Color == c_iWhiteColor;
        bool bIf6 = pShape->m_bIsNoFill == false;
        bool bIf7 = pShape->m_bIsNoStroke == true;

        if (bIf1 && bIf2 && bIf3 && bIf4 && !bIf5 && bIf6 && bIf7)
        {
            //Удовлетворяет расположением и размером - привязываем указатель на картинку
            pCont->m_pShape = pShape;
            pCont->m_bIsHighlightPresent = true;
            pCont->m_lHighlightColor = pShape->m_oBrush.Color1;
            return true;
        }

        return false;
    }

    void CPage::AddDiacriticalSymbols()
    {
        if (m_arDiacriticalSymbol.empty())
        {
            return;
        }

        for (size_t i = 0; i < m_arDiacriticalSymbol.size(); ++i)
        {
            auto pDiacriticalCont = m_arDiacriticalSymbol[i];
            if (pDiacriticalCont->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            bool isBreak = false;

            for (size_t j = 0; j < m_arTextLine.size(); ++j)
            {
                auto pCurrLine = m_arTextLine[j];

                if (pCurrLine->m_bIsNotNecessaryToUse ||
                    pCurrLine->AreObjectsNoCrossing(pDiacriticalCont))
                {
                    continue;
                }

                for (size_t k = 0; k < pCurrLine->m_arConts.size(); ++k)
                {
                    auto pCurrCont = pCurrLine->m_arConts[k];

                    if (pCurrCont->m_bIsNotNecessaryToUse)
                    {
                        continue;
                    }

                    eVerticalCrossingType eVType = pCurrCont->GetVerticalCrossingType(pDiacriticalCont);
                    eHorizontalCrossingType eHType = pCurrCont->GetHorizontalCrossingType(pDiacriticalCont);

                    if (eVType != eVerticalCrossingType::vctNoCrossingCurrentAboveNext &&
                        eVType != eVerticalCrossingType::vctNoCrossingCurrentBelowNext &&
                        eHType != eHorizontalCrossingType::hctNoCrossingCurrentLeftOfNext &&
                        eHType != eHorizontalCrossingType::hctNoCrossingCurrentRightOfNext)
                    {
                        bool bIf1 = eHType == eHorizontalCrossingType::hctCurrentOutsideNext;
                        bool bIf2 = eHType == eHorizontalCrossingType::hctCurrentLeftOfNext;
                        bool bIf3 = eHType == eHorizontalCrossingType::hctCurrentRightOfNext;
                        bool bIf4 = eHType == eHorizontalCrossingType::hctDublicate;
                        bool bIf5 = eHType == eHorizontalCrossingType::hctRightBorderMatch;

                        bool bIf6 = eVType == eVerticalCrossingType::vctCurrentBelowNext ||
                                    eVType == eVerticalCrossingType::vctCurrentAboveNext;
                        bool bIf7 = eVType == eVerticalCrossingType::vctTopAndBottomBordersMatch;
                        bool bIf8 = eVType == eVerticalCrossingType::vctDublicate;

                        if ((bIf1 && bIf6) || (bIf2 && bIf7) || (bIf4 && bIf8) || (bIf5 && bIf7))
                        {
                            pCurrCont->m_oText += pDiacriticalCont->m_oText;
                            pDiacriticalCont->m_bIsNotNecessaryToUse = true;
                            isBreak = true;
                            break;
                        }
                        else if (bIf3 && bIf7)
                        {
                            NSStringUtils::CStringUTF32 oText(pDiacriticalCont->m_oText);
                            oText += pCurrCont->m_oText;
                            pCurrCont->m_oText = oText;
                            pDiacriticalCont->m_bIsNotNecessaryToUse = true;
                            isBreak = true;
                            break;
                        }
                    }
                }
                if (isBreak)
                {
                    break;
                }
            }
        }
    }

    void CPage::MergeLinesByVertAlignType()
    {
        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            auto pCurrLine = m_arTextLine[i];

            if (pCurrLine->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            if (pCurrLine->m_eVertAlignType == eVertAlignType::vatSuperscript)
            {
                auto pBaseLine = pCurrLine->m_pLine;
                if (pBaseLine)
                {
                    double dFontSize = 0;

                    for (size_t j = 0; j < pBaseLine->m_arConts.size(); ++j)
                    {
                        auto pCont = pBaseLine->m_arConts[j];
                        if (pCont->m_bIsNotNecessaryToUse || !pCont->m_pCont)
                        {
                            continue;
                        }

                        dFontSize = pCont->m_pFontStyle->m_oFont.Size;
                        break;
                    }

                    for (auto pCont : pCurrLine->m_arConts)
                    {
                        m_pStyleManager->m_pCurrentStyle->CopyFormat(*pCont->m_pFontStyle);
                        m_pStyleManager->m_pCurrentStyle->m_oFont.Size = dFontSize;
                        pCont->m_pFontStyle = m_pStyleManager->GetStyle();

                        if (pBaseLine->m_dLeft > pCont->m_dLeft)
                        {
                            pBaseLine->m_dLeft = pCont->m_dLeft;
                        }
                        if (pBaseLine->m_dRight < pCont->m_dRight)
                        {
                            pBaseLine->m_dRight = pCont->m_dRight;
                        }

                        pBaseLine->m_dWidth = pBaseLine->m_dRight - pBaseLine->m_dLeft;

                        pBaseLine->m_arConts.push_back(std::move(pCont));
                    }
                    SortElements(pBaseLine->m_arConts);
                    pCurrLine->m_bIsNotNecessaryToUse = true;
                }
            }
            else if (pCurrLine->m_eVertAlignType == eVertAlignType::vatBase)
            {
                auto pSubLine = pCurrLine->m_pLine;
                if (pSubLine)
                {
                    double dFontSize = 0;

                    for (size_t j = 0; j < pCurrLine->m_arConts.size(); ++j)
                    {
                        auto pCont = pCurrLine->m_arConts[j];
                        if (pCont->m_bIsNotNecessaryToUse || !pCont->m_pCont)
                        {
                            continue;
                        }

                        dFontSize = pCont->m_pFontStyle->m_oFont.Size;
                        break;
                    }

                    for (auto pCont : pSubLine->m_arConts)
                    {
                        m_pStyleManager->m_pCurrentStyle->CopyFormat(*pCont->m_pFontStyle);
                        m_pStyleManager->m_pCurrentStyle->m_oFont.Size = dFontSize;
                        pCont->m_pFontStyle = m_pStyleManager->GetStyle();

                        if (pCurrLine->m_dLeft > pCont->m_dLeft)
                        {
                            pCurrLine->m_dLeft = pCont->m_dLeft;
                        }
                        if (pCurrLine->m_dRight < pCont->m_dRight)
                        {
                            pCurrLine->m_dRight = pCont->m_dRight;
                        }

                        pCurrLine->m_dWidth = pCurrLine->m_dRight - pCurrLine->m_dLeft;

                        pCurrLine->m_arConts.push_back(std::move(pCont));
                    }
                    SortElements(pCurrLine->m_arConts);
                    pSubLine->m_bIsNotNecessaryToUse = true;
                }
            }
        }
    }

    void CPage::BuildLines()
    {
        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            auto pCurrLine = m_arTextLine[i];
            if (pCurrLine->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            for (size_t j = 0; j < pCurrLine->m_arConts.size(); ++j)
            {
                auto pCurrCont = pCurrLine->m_arConts[j];
                if (pCurrCont->m_bIsNotNecessaryToUse)
                {
                    continue;
                }

                if (pCurrCont->m_iNumDuplicates > 0)
                {
                    pCurrLine->m_iNumDuplicates = std::max(pCurrLine->m_iNumDuplicates, pCurrCont->m_iNumDuplicates);
                }
            }

            pCurrLine->MergeConts();
        }
    }

    void CPage::DetermineDominantGraphics()
    {
        CShape* pDominantShape = nullptr;

        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            auto pLine = m_arTextLine[i];
            if (pLine->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            for (size_t j = 0; j < pLine->m_arConts.size(); ++j)
            {
                auto pCont = pLine->m_arConts[j];
                if (pCont->m_bIsNotNecessaryToUse)
                {
                    continue;
                }

                if (pCont->m_pShape && pCont->m_pShape != pDominantShape)
                {
                    if (pCont->m_pShape->m_dLeft < pCont->m_dLeft &&
                        pCont->m_pShape->m_dRight > pCont->m_dRight)
                    {
                        if (!pDominantShape ||
                            (pCont->m_pShape->m_dLeft < pDominantShape->m_dLeft &&
                             pCont->m_pShape->m_dRight > pDominantShape->m_dRight))
                        {
                            pDominantShape = pCont->m_pShape;
                        }
                    }
                }
            }

            pLine->m_pDominantShape = pDominantShape;
            pDominantShape = nullptr;
        }
    }

    void CPage::BuildByType()
    {
        if (m_arTextLine.empty())
            return;

        switch (m_eTextAssociationType)
        {
        case tatBlockChar:
            BuildByTypeBlockChar();
            break;
        case tatBlockLine:
            BuildByTypeBlockLine();
            break;
        case tatPlainLine:
            BuildByTypePlainLine();
            break;
        case tatShapeLine:
            BuildByTypeShapeLine();
            break;
        case tatPlainParagraph:
        case tatParagraphToShape:
            BuildByTypePlainParagraph();
            break;
        default:
            break;
        }
    }

    void CPage::BuildByTypeBlockChar()
    {
        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            auto pLine = m_arTextLine[i];
            auto pParagraph = new CParagraph(m_eTextAssociationType);
            pParagraph->m_eTextConversionType = CParagraph::tctTextToFrame;

            pParagraph->m_dLeft = pLine->m_dLeft;
            pParagraph->m_dTop	= pLine->m_dBaselinePos - pLine->m_dHeight;

            pParagraph->m_arLines.push_back(pLine);

            m_arParagraphs.push_back(pParagraph);
        }
    }

    void CPage::BuildByTypeBlockLine()
    {
        auto pFirstLine = m_arTextLine[0];

        auto pParagraph = new CParagraph(m_eTextAssociationType);
        pParagraph->m_eTextConversionType = CParagraph::tctTextToFrame;

        pParagraph->m_dLeft	= pFirstLine->m_dLeft;
        pParagraph->m_dTop	= pFirstLine->m_dBaselinePos - pFirstLine->m_dHeight;
        double dCurrentTop = pParagraph->m_dTop;

        pParagraph->m_arLines.push_back(pFirstLine);

        m_arParagraphs.push_back(pParagraph);

        for (size_t i = 1; i < m_arTextLine.size(); ++i)
        {
            auto pTextLine = m_arTextLine[i];

            auto pParagraph = new CParagraph(m_eTextAssociationType);
            pParagraph->m_eTextConversionType = CParagraph::tctTextToFrame;

            if (((fabs(pTextLine->m_dBaselinePos - pTextLine->m_dHeight - pFirstLine->m_dBaselinePos) > c_dSTANDART_STRING_HEIGHT_MM) && (pTextLine->m_dLeft == pFirstLine->m_dLeft)) ||
                ((pTextLine->m_dLeft != pFirstLine->m_dLeft) && (pTextLine->m_dBaselinePos != pFirstLine->m_dBaselinePos)))
            {
                pParagraph->m_dLeft	= pTextLine->m_dLeft;
                pParagraph->m_dTop	= pTextLine->m_dBaselinePos - pTextLine->m_dHeight;
                dCurrentTop = pParagraph->m_dTop;
            }
            else
            {
                pParagraph->m_dLeft	= pFirstLine->m_dLeft;
                pParagraph->m_dTop	= dCurrentTop;
            }

            pFirstLine = pTextLine;

            pParagraph->m_arLines.push_back(pTextLine);
            m_arParagraphs.push_back(pParagraph);
        }
    }

    void CPage::BuildByTypePlainLine()
    {
        Merge(c_dSTANDART_STRING_HEIGHT_MM / 3);

        double dPreviousStringBaseline = 0;
        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            auto pLine = m_arTextLine[i];
            double dBeforeSpacing = pLine->CalculateBeforeSpacing(dPreviousStringBaseline);
            dPreviousStringBaseline = pLine->m_dBaselinePos;
            double dRight = pLine->CalculateRightBorder(m_dWidth);

            CreateSingleLineParagraph(pLine, &dRight, &dBeforeSpacing);
        }
    }

    void CPage::BuildByTypeShapeLine()
    {
        for (size_t i = 0; i < m_arTextLine.size(); ++i)
        {
            CreateSingleLineShape(m_arTextLine[i]);
        }
    }

    void CPage::BuildByTypePlainParagraph()
    {
        CTextLine* pCurrLine, *pNextLine, *pNextNextLine, *pPrevLine;
        double dCurrRight = 0, dNextRight = 0;
        double dCurrBeforeSpacing = 0, dNextBeforeSpacing = 0, dPrevBeforeSpacing = 0;
        double dBeforeSpacingWithShapes = 0;
        //note Все параграфы были сдвинуты на данное значение от верхнего края страницы
        double dPreviousStringBaseline = c_dCORRECTION_FOR_FIRST_PARAGRAPH;
        eVerticalCrossingType eCrossingType;

        bool bIf1, bIf2, bIf3, bIf4, bIf5, bIf6, bIf7;

        size_t nIndexForCheking = c_nAntiZero;

        for (size_t nIndex = 0; nIndex < m_arTextLine.size(); ++nIndex)
        {
            pCurrLine = m_arTextLine[nIndex];
            if (pCurrLine->m_bIsNotNecessaryToUse)
            {
                continue;
            }

            dPrevBeforeSpacing = dCurrBeforeSpacing;
            dCurrBeforeSpacing = pCurrLine->CalculateBeforeSpacing(dPreviousStringBaseline);
            dPreviousStringBaseline = pCurrLine->m_dBaselinePos;

            if (pCurrLine->m_iNumDuplicates > 0)
            {
                dBeforeSpacingWithShapes += dCurrBeforeSpacing + pCurrLine->m_dHeight;

                auto iNumDuplicates = pCurrLine->m_iNumDuplicates;
                CreateSingleLineShape(pCurrLine);
                while (iNumDuplicates > 0)
                {
                    CreateSingleLineShape(pCurrLine);
                    iNumDuplicates--;
                }
                continue;
            }

            dCurrRight = pCurrLine->CalculateRightBorder(m_dWidth);
            pNextLine = GetNextTextLine(nIndex, &nIndexForCheking);
            if (m_eTextAssociationType == tatParagraphToShape)
            {
                pPrevLine = GetPrevTextLine(nIndex);
            }

            if (pNextLine)
            {
                eCrossingType = pCurrLine->GetVerticalCrossingType(pNextLine);
                bool bIsPassed = false;
                double dCurrentAdditive = 0.0;

                switch (eCrossingType)
                {
                case eVerticalCrossingType::vctCurrentInsideNext:
                case eVerticalCrossingType::vctCurrentBelowNext:
                    dCurrentAdditive = dCurrBeforeSpacing + pCurrLine->m_dHeight + pNextLine->m_dBaselinePos - pCurrLine->m_dBaselinePos;
                    dPreviousStringBaseline = pNextLine->m_dBaselinePos;
                    bIsPassed = true;
                    break;
                case eVerticalCrossingType::vctCurrentOutsideNext:
                case eVerticalCrossingType::vctCurrentAboveNext:
                case eVerticalCrossingType::vctDublicate:
                    dCurrentAdditive = dCurrBeforeSpacing + pCurrLine->m_dHeight;
                    bIsPassed = true;
                    break;
                default:
                    break;
                }

                if (bIsPassed)
                {
                    CreateSingleLineShape(pCurrLine);
                    CreateSingleLineShape(pNextLine);

                    dBeforeSpacingWithShapes += dCurrentAdditive;

                    nIndex++;
                    continue;
                }
            }

            dCurrBeforeSpacing += dBeforeSpacingWithShapes;
            dBeforeSpacingWithShapes = 0;

            bool bIsSingleLineParagraph = false;

            if (pNextLine)
            {
                dNextRight = pNextLine->CalculateRightBorder(m_dWidth);
                dNextBeforeSpacing = pNextLine->CalculateBeforeSpacing(dPreviousStringBaseline);

                //Высота строк должна быть примерно одинаковой
                bIf1 = fabs(pCurrLine->m_dHeight - pNextLine->m_dHeight) < c_dTHE_SAME_STRING_Y_PRECISION_MM;
                //расстрояние между строк тоже одинаково
                bIf2 = fabs(dCurrBeforeSpacing - dNextBeforeSpacing) < c_dLINE_DISTANCE_ERROR_MM;
                //или
                bIf3 = dCurrBeforeSpacing > dNextBeforeSpacing;
                //нет отступа
                bIf4 = fabs(pCurrLine->m_dLeft - pNextLine->m_dLeft) < c_dERROR_OF_PARAGRAPH_BORDERS_MM;
                //есть отступ
                bIf5 = pCurrLine->m_dLeft > pNextLine->m_dLeft;
                //совпадают правые границы
                bIf6 = fabs(dCurrRight - dNextRight) < c_dERROR_OF_PARAGRAPH_BORDERS_MM;

                size_t nNextIndex = nIndex+1;
                pNextNextLine = GetNextTextLine(nNextIndex);

                bIf7 = (pCurrLine->m_dWidth > pNextLine->m_dWidth * c_dCOEFFICIENT_LENGTHS_LINES_IN_PARAGRAPH) &&
                        (pNextNextLine ? pCurrLine->m_dWidth > pNextNextLine->m_dWidth * c_dCOEFFICIENT_LENGTHS_LINES_IN_PARAGRAPH : true);

                if (pNextNextLine)
                {
                    double dNextNextBeforeSpacing = pNextNextLine->CalculateBeforeSpacing(pNextLine->m_dBaselinePos);

                    if (bIf1 && (bIf2 || bIf3))
                    {
                        if (fabs(dNextBeforeSpacing - dNextNextBeforeSpacing) < c_dLINE_DISTANCE_ERROR_MM)
                        {
                            if (fabs(pNextLine->m_dHeight - pNextNextLine->m_dHeight) >= c_dTHE_SAME_STRING_Y_PRECISION_MM)
                            {
                                pNextNextLine = nullptr;
                            }
                        }
                        else
                        {
                            if (fabs(pNextLine->m_dHeight - pNextNextLine->m_dHeight) < c_dTHE_SAME_STRING_Y_PRECISION_MM)
                            {
                                if (dNextBeforeSpacing < dNextNextBeforeSpacing)
                                {
                                    pNextNextLine = nullptr;
                                }
                                else
                                {
                                    bIsSingleLineParagraph = true;
                                }
                            }
                            else
                            {
                                pNextNextLine = nullptr;
                            }
                        }
                    }
                }
            }

            bool bIsUseNextNextLine = true;
            CParagraph::TextAlignmentType eTextAlignmentType = CParagraph::DetermineTextAlignmentType(
                        pCurrLine, pNextLine, pNextNextLine, m_dWidth, bIsUseNextNextLine, bIsSingleLineParagraph);

            auto pParagraph = new CParagraph(m_eTextAssociationType);
            pParagraph->m_eTextAlignmentType = eTextAlignmentType;
            if (m_eTextAssociationType == tatPlainParagraph)
            {
                pParagraph->m_eTextConversionType = CParagraph::tctTextToParagraph;
            }
            else
            {
                pParagraph->m_eTextConversionType = CParagraph::tctTextToShape;
            }

            if (pNextLine && !bIsSingleLineParagraph && bIf1 && (bIf2 || bIf3))
            {
                pParagraph->m_dLeft = std::min(pCurrLine->m_dLeft, pNextLine->m_dLeft);
                pParagraph->m_dRight = std::min(dCurrRight, dNextRight);
                pParagraph->m_dWidth = std::max(pCurrLine->m_dWidth + pCurrLine->m_arConts.back()->m_dSpaceWidthMM,
                                                pNextLine->m_dWidth + pNextLine->m_arConts.back()->m_dSpaceWidthMM);
                if (pParagraph->m_eTextAlignmentType != CParagraph::tatByCenter)
                {
                    pParagraph->m_bIsNeedFirstLineIndent = pCurrLine->m_dLeft > pNextLine->m_dLeft ? true : false;
                    pParagraph->m_dFirstLine = pCurrLine->m_dLeft - pNextLine->m_dLeft;
                }
            }
            else
            {
                pParagraph->m_dLeft = pCurrLine->m_dLeft;
                pParagraph->m_dRight = dCurrRight;
                pParagraph->m_dWidth = pCurrLine->m_dWidth;
            }

            pParagraph->m_dTop = pCurrLine->m_dBaselinePos - pCurrLine->m_dHeight;

            //размер строк во всем параграфе
            pParagraph->m_dHeight = pCurrLine->m_dHeight;
            pParagraph->m_dSpaceBefore = std::max(dCurrBeforeSpacing, 0.0);

            pParagraph->m_arLines.push_back(pCurrLine);
            pParagraph->m_nNumLines++;

            if (pNextLine && !bIsSingleLineParagraph && bIf1 && (bIf2 || bIf3) && (bIf4 || bIf5 || bIf6) && bIf7)
            {
                pParagraph->m_arLines.push_back(pNextLine);
                pParagraph->m_nNumLines++;

                if (IsShadingPresent(pCurrLine, pNextLine))
                {
                    pParagraph->m_bIsShadingPresent = true;
                    pParagraph->m_lColorOfShadingFill = pCurrLine->m_pDominantShape->m_oBrush.Color1;
                }

                //сдвигаем рабочую точку
                nIndex++;
                pCurrLine = pNextLine;
                pNextLine = GetNextTextLine(nIndex, &nIndexForCheking);

                dPrevBeforeSpacing = dCurrBeforeSpacing;
                dCurrBeforeSpacing = pCurrLine->CalculateBeforeSpacing(dPreviousStringBaseline);
                dPreviousStringBaseline = pCurrLine->m_dBaselinePos;
                double dCorrectionBeforeSpacing = dCurrBeforeSpacing;
                dCurrRight = dNextRight;

                if (bIsUseNextNextLine)
                {
                    if (pNextLine)
                    {
                        dNextBeforeSpacing = pNextLine->CalculateBeforeSpacing(dPreviousStringBaseline);
                        dNextRight = pNextLine->CalculateRightBorder(m_dWidth);
                        eCrossingType = pCurrLine->GetVerticalCrossingType(pNextLine);

                        bIf1 = fabs(pCurrLine->m_dHeight - pNextLine->m_dHeight) < c_dTHE_SAME_STRING_Y_PRECISION_MM; //высота строк должна быть примерно одинаковой
                        bIf2 = fabs(dCurrBeforeSpacing - dNextBeforeSpacing) < c_dLINE_DISTANCE_ERROR_MM; //расстрояние между строк тоже одинаково
                        bIf3 = (eCrossingType == eVerticalCrossingType::vctUnknown ||
                                eCrossingType == eVerticalCrossingType::vctNoCrossingCurrentAboveNext ||
                                eCrossingType == eVerticalCrossingType::vctNoCrossingCurrentBelowNext);
                        bIf4 = ((pParagraph->m_eTextAlignmentType == CParagraph::tatByLeftEdge && fabs(pCurrLine->m_dLeft - pNextLine->m_dLeft) < c_dERROR_OF_PARAGRAPH_BORDERS_MM) ||
                                (pParagraph->m_eTextAlignmentType == CParagraph::tatByWidth && fabs(pCurrLine->m_dLeft - pNextLine->m_dLeft) < c_dERROR_OF_PARAGRAPH_BORDERS_MM && (fabs(dCurrRight - dNextRight) < c_dERROR_OF_PARAGRAPH_BORDERS_MM || dCurrRight < dNextRight)) ||
                                (pParagraph->m_eTextAlignmentType == CParagraph::tatByRightEdge && fabs(dCurrRight - dNextRight) < c_dERROR_OF_PARAGRAPH_BORDERS_MM) ||
                                (pParagraph->m_eTextAlignmentType == CParagraph::tatByCenter));
                        bIf5 = (pCurrLine->m_dWidth > pNextLine->m_dWidth * c_dCOEFFICIENT_LENGTHS_LINES_IN_PARAGRAPH);
                    }

                    //проверим, подходят ли следующие строчки для текущего pParagraph
                    while(pNextLine &&  bIf1 && bIf2 && bIf3 && bIf4 && bIf5)
                    {
                        pParagraph->m_arLines.push_back(pNextLine);
                        pParagraph->m_nNumLines++;

                        pParagraph->m_dRight = std::min(pParagraph->m_dRight, dNextRight);
                        pParagraph->m_dLeft = std::min(pParagraph->m_dLeft, pNextLine->m_dLeft);
                        pParagraph->m_dWidth = std::max(pParagraph->m_dWidth, pNextLine->m_dWidth + pNextLine->m_arConts.back()->m_dSpaceWidthMM);

                        if (!IsShadingPresent(pCurrLine, pNextLine))
                        {
                            pParagraph->m_bIsShadingPresent = false;
                            pParagraph->m_lColorOfShadingFill = c_iWhiteColor;
                        }

                        //сдвигаем рабочую точку
                        nIndex++;
                        pCurrLine = pNextLine;
                        pNextLine = GetNextTextLine(nIndex, &nIndexForCheking);

                        dPrevBeforeSpacing = dCurrBeforeSpacing;
                        dCurrBeforeSpacing = pCurrLine->CalculateBeforeSpacing(dPreviousStringBaseline);
                        dPreviousStringBaseline = pCurrLine->m_dBaselinePos;
                        dCorrectionBeforeSpacing = (dCorrectionBeforeSpacing + dCurrBeforeSpacing)/2; //наверное лучше так... текст может быть уже, чем в оригинале
                        dCurrRight = dNextRight;

                        if (pNextLine)
                        {
                            dNextBeforeSpacing = pNextLine->CalculateBeforeSpacing(dPreviousStringBaseline);
                            dNextRight = pNextLine->CalculateRightBorder(m_dWidth);
                            eCrossingType = pCurrLine->GetVerticalCrossingType(pNextLine);

                            bIf1 = fabs(pCurrLine->m_dHeight - pNextLine->m_dHeight) < c_dTHE_SAME_STRING_Y_PRECISION_MM; //высота строк должна быть примерно одинаковой
                            bIf2 = fabs(dCurrBeforeSpacing - dNextBeforeSpacing) < c_dLINE_DISTANCE_ERROR_MM; //расстрояние между строк тоже одинаково
                            bIf3 = (eCrossingType == eVerticalCrossingType::vctUnknown ||
                                    eCrossingType == eVerticalCrossingType::vctNoCrossingCurrentAboveNext ||
                                    eCrossingType == eVerticalCrossingType::vctNoCrossingCurrentBelowNext);
                            bIf4 = ((pParagraph->m_eTextAlignmentType == CParagraph::tatByLeftEdge && fabs(pCurrLine->m_dLeft - pNextLine->m_dLeft) < c_dERROR_OF_PARAGRAPH_BORDERS_MM) ||
                                    (pParagraph->m_eTextAlignmentType == CParagraph::tatByWidth && fabs(pCurrLine->m_dLeft - pNextLine->m_dLeft) < c_dERROR_OF_PARAGRAPH_BORDERS_MM && (fabs(dCurrRight - dNextRight) < c_dERROR_OF_PARAGRAPH_BORDERS_MM || dCurrRight < dNextRight)) ||
                                    (pParagraph->m_eTextAlignmentType == CParagraph::tatByRightEdge && fabs(dCurrRight - dNextRight) < c_dERROR_OF_PARAGRAPH_BORDERS_MM) ||
                                    (pParagraph->m_eTextAlignmentType == CParagraph::tatByCenter));
                            bIf5 = (pCurrLine->m_dWidth > pNextLine->m_dWidth * c_dCOEFFICIENT_LENGTHS_LINES_IN_PARAGRAPH);
                        }
                    }
                }

                if (eCrossingType != eVerticalCrossingType::vctUnknown &&
                    eCrossingType != eVerticalCrossingType::vctNoCrossingCurrentAboveNext &&
                    eCrossingType != eVerticalCrossingType::vctNoCrossingCurrentBelowNext)
                {
                    CreateSingleLineShape(pNextLine);
                    nIndex++;
                }

                //коррекция
                pParagraph->m_dHeight += dCorrectionBeforeSpacing;
                pParagraph->m_dSpaceBefore = fabs(pParagraph->m_dSpaceBefore - dCorrectionBeforeSpacing);

                pParagraph->RemoveHighlightColor();
                pParagraph->MergeLines();
            }
            else
            {
                if (pCurrLine->m_pDominantShape)
                {
                    pParagraph->m_bIsShadingPresent = true;
                    pParagraph->m_lColorOfShadingFill = pCurrLine->m_pDominantShape->m_oBrush.Color1;
                    pParagraph->RemoveHighlightColor();
                }
            }

            if (m_eTextAssociationType == tatParagraphToShape)
            {
                bool bIsSameTypeText = pPrevLine && fabs(dPrevBeforeSpacing - dCurrBeforeSpacing) < c_dLINE_DISTANCE_ERROR_MM;
                CreateShapeFormParagraphs(pParagraph, bIsSameTypeText);
            }
            else
            {
                m_arParagraphs.push_back(pParagraph);
            }

            if (nIndexForCheking != c_nAntiZero)
            {
                nIndex = nIndexForCheking - 1;
                nIndexForCheking = c_nAntiZero;
            }
        }

        if (m_eTextAssociationType == tatParagraphToShape)
        {
            CorrectionParagraphsInShapes();
        }
    }

    CTextLine* CPage::GetNextTextLine(size_t& nCurrentIndex, size_t* pIndexForCheking)
    {
        CTextLine* pLine = nullptr;

        for (size_t nIndex = nCurrentIndex + 1; nIndex < m_arTextLine.size(); ++nIndex)
        {
            pLine = m_arTextLine[nIndex];
            bool bIf1 = pLine->m_bIsNotNecessaryToUse;
            bool bIf2 = pIndexForCheking && pLine->m_iNumDuplicates > 0;

            if (bIf1 || bIf2)
            {
                if (bIf2)
                {
                    if (*pIndexForCheking == c_nAntiZero)
                    {
                        *pIndexForCheking = nIndex;
                    }
                }

                nCurrentIndex++; //note изменяем входной индекс, чтобы не выбирать те же строки
                pLine = nullptr;
                continue;
            }
            else
            {
                break;
            }
        }
        return pLine;
    }

    CTextLine* CPage::GetPrevTextLine(size_t nCurrentIndex)
    {
        CTextLine* pLine = nullptr;

        if (nCurrentIndex)
        {
            for (size_t nIndex = nCurrentIndex - 1; nIndex > 0; --nIndex)
            {
                pLine = m_arTextLine[nIndex];

                if (pLine->m_bIsNotNecessaryToUse)
                {
                    pLine = nullptr;
                    continue;
                }
                else
                {
                    break;
                }
            }
        }
        return pLine;
    }

    bool CPage::IsShadingPresent(const CTextLine *pLine1, const CTextLine *pLine2)
    {
        if (pLine1->m_pDominantShape && pLine2->m_pDominantShape &&
            pLine1->m_pDominantShape->m_oBrush.Color1 == pLine2->m_pDominantShape->m_oBrush.Color1 &&
            fabs(pLine1->m_pDominantShape->m_dLeft - pLine2->m_pDominantShape->m_dLeft) < c_dGRAPHICS_ERROR_IN_LINES_MM &&
            fabs(pLine1->m_pDominantShape->m_dWidth - pLine2->m_pDominantShape->m_dWidth) < c_dGRAPHICS_ERROR_IN_LINES_MM)
        {
            return true;
        }

        return false;
    }

    void CPage::CreateSingleLineParagraph(CTextLine *pLine, const double *pRight, const double *pBeforeSpacing)
    {
        auto pParagraph = new CParagraph(m_eTextAssociationType);
        pParagraph->m_eTextConversionType = CParagraph::tctTextToParagraph;
        pParagraph->m_arLines.push_back(pLine);

        pParagraph->m_dLeft	= pLine->m_dLeft;
        pParagraph->m_dTop	= pLine->m_dTop;
        pParagraph->m_dFirstLine = 0;
        pParagraph->m_dRight = *pRight;
        pParagraph->m_dWidth = pLine->m_dWidth;
        pParagraph->m_dHeight = pLine->m_dHeight;
        if (*pBeforeSpacing < 0)
        {
            pParagraph->m_dHeight += *pBeforeSpacing;
        }

        pParagraph->m_dSpaceBefore = std::max(*pBeforeSpacing, 0.0);

        if (pLine->m_pDominantShape)
        {
            pParagraph->m_bIsShadingPresent = true;
            pParagraph->m_lColorOfShadingFill = pLine->m_pDominantShape->m_oBrush.Color1;
            pParagraph->RemoveHighlightColor();
        }

        m_arParagraphs.push_back(pParagraph);
    }

    void CPage::CreateSingleLineOldShape(CTextLine *pLine)
    {
        auto pParagraph = new CParagraph(m_eTextAssociationType);
        pParagraph->m_eTextConversionType = CParagraph::tctTextToShape;
        pParagraph->m_arLines.push_back(pLine);

        if (pLine->m_pDominantShape)
        {
            pParagraph->m_bIsShadingPresent = true;
            pParagraph->m_lColorOfShadingFill = pLine->m_pDominantShape->m_oBrush.Color1;
            pParagraph->RemoveHighlightColor();
        }

        auto pShape = std::make_shared<COldShape>();
        pShape->m_arParagraphs.push_back(pParagraph);

        //todo В итоге Left и Top смещаются на несколько мм. не использовать margin-left и margin-top.
        pShape->m_dLeft	= pLine->m_dLeft - COldShape::POSITION_CORRECTION_FOR_X_MM; //подобранная константа
        pShape->m_dTop	= pLine->m_dBaselinePos - pLine->m_dHeight - COldShape::POSITION_CORRECTION_FOR_Y_MM;//подобранная константа
        pShape->m_dWidth = pLine->m_dWidth + COldShape::SIZE_CORRECTION_FOR_X_MM; //5мм мало для длинной строки
        pShape->m_dHeight = pLine->m_dHeight + COldShape::SIZE_CORRECTION_FOR_Y_MM;

        //m_arGraphicItems.push_back(pShape);
    }

    void CPage::CreateSingleLineShape(CTextLine *pLine)
    {
        auto pParagraph = new CParagraph(m_eTextAssociationType);
        pParagraph->m_eTextConversionType = CParagraph::tctTextToShape;
        pParagraph->m_arLines.push_back(pLine);

        if (pLine->m_pDominantShape)
        {
            pParagraph->m_bIsShadingPresent = true;
            pParagraph->m_lColorOfShadingFill = pLine->m_pDominantShape->m_oBrush.Color1;
            pParagraph->RemoveHighlightColor();
        }

        auto pShape = new CShape();
        pShape->m_arParagraphs.push_back(pParagraph);
        pShape->m_eType = CShape::eShapeType::stTextBox;
        pShape->m_dLeft = pLine->m_dLeft;
        pShape->m_dTop = pLine->m_dTop;
        pShape->m_dWidth = pLine->m_dWidth;
        pShape->m_dHeight = pLine->m_dHeight;
        pShape->m_bIsBehindDoc = false;

        m_arShapes.push_back(pShape);
    }

    void CPage::CreateShapeFormParagraphs(CParagraph* pParagraph, bool bIsSameTypeText)
    {
       if (!pParagraph)
       {
           return;
       }

       CShape* pShape;

       if (bIsSameTypeText && !m_arShapes.empty())
       {
           pShape = m_arShapes.back();
           pShape->m_dHeight += pParagraph->m_dHeight * pParagraph->m_nNumLines + pParagraph->m_dSpaceBefore;
       }
       else
       {
           pShape = new CShape();
           pParagraph->m_dSpaceBefore = 0;
           pShape->m_dHeight += pParagraph->m_dHeight * pParagraph->m_nNumLines;
       }

       if (pShape->m_dLeft > 0)
       {
           pShape->m_dLeft = std::min(pShape->m_dLeft, pParagraph->m_dLeft);
       }
       else
       {
           pShape->m_dLeft = pParagraph->m_dLeft;
       }

       if (pShape->m_dTop > 0)
       {
           pShape->m_dTop = std::min(pShape->m_dTop, pParagraph->m_dTop);
       }
       else
       {
           pShape->m_dTop = pParagraph->m_dTop;
       }

       if (pShape->m_dRight > 0)
       {
           pShape->m_dRight = std::min(pShape->m_dRight, pParagraph->m_dRight);
       }
       else
       {
           pShape->m_dRight = pParagraph->m_dRight;
       }

       pShape->m_dWidth = m_dWidth - pShape->m_dLeft - pShape->m_dRight;

       pShape->m_arParagraphs.push_back(pParagraph);
       pShape->m_eType = CShape::eShapeType::stTextBox;
       pShape->m_bIsBehindDoc = false;

       if (!bIsSameTypeText)
       {
           m_arShapes.push_back(pShape);
       }
    }

    void CPage::CorrectionParagraphsInShapes()
    {
        for (size_t i = 0; i <  m_arShapes.size(); ++i)
        {
            auto pShape = m_arShapes[i];
            if (pShape->m_bIsNotNecessaryToUse ||
                pShape->m_eType != CShape::eShapeType::stTextBox ||
                pShape->m_arParagraphs.empty())
            {
                continue;
            }

            for (size_t j = 0; j < pShape->m_arParagraphs.size(); ++j)
            {
                auto pParagraph = pShape->m_arParagraphs[j];
                if (pParagraph->m_dLeft > pShape->m_dLeft && pParagraph->m_nNumLines == 1)
                {
                    pParagraph->m_bIsNeedFirstLineIndent = true;
                    pParagraph->m_dFirstLine = pParagraph->m_dLeft - pShape->m_dLeft;
                    pParagraph->m_dLeft = 0;
                }

                pParagraph->m_dLeft = pParagraph->m_dLeft > pShape->m_dLeft ? pParagraph->m_dLeft - pShape->m_dLeft : 0 ;
                pParagraph->m_dRight = pParagraph->m_dRight - pShape->m_dRight;
            }
        }
    }

    void CPage::Merge(double dAffinity)
    {
        size_t nCount = m_arTextLine.size();
        if (1 < nCount)
        {
            auto pPrev = m_arTextLine[0];
            for (size_t i = 1; i < nCount; ++i)
            {
                auto pNext = m_arTextLine[i];

                if (fabs(pNext->m_dBaselinePos - pPrev->m_dBaselinePos) < dAffinity)
                {
                    pPrev->Merge(pNext);
                    pNext->m_bIsNotNecessaryToUse = true;
                    continue;
                }
                pPrev = pNext;
            }
        }
    }

    void CPage::ToXml(NSStringUtils::CStringBuilder& oWriter)
    {
        if (!m_arImages.empty())
        {
            oWriter.WriteString(L"<w:p>");
            //note при удалении строки откуда-то добавляется <w:p/> в начале страницы (если есть графика и текст), что добавляет дополнительную строку и сдвигает текст
            oWriter.WriteString(L"<w:pPr><w:spacing w:line=\"1\" w:lineRule=\"exact\"/></w:pPr>");

            for (size_t i = 0; i < m_arImages.size(); ++i)
            {
                m_arImages[i]->ToXml(oWriter);
            }

            oWriter.WriteString(L"</w:p>");
        }

        // drawings
        if (!m_arShapes.empty())
        {
            oWriter.WriteString(L"<w:p>");
            //note при удалении строки откуда-то добавляется <w:p/> в начале страницы (если есть графика и текст), что добавляет дополнительную строку и сдвигает текст
            oWriter.WriteString(L"<w:pPr><w:spacing w:line=\"1\" w:lineRule=\"exact\"/></w:pPr>");

            for (size_t i = 0; i < m_arShapes.size(); ++i)
            {
                m_arShapes[i]->ToXml(oWriter);
            }

            oWriter.WriteString(L"</w:p>");
        }

        for (size_t i = 0; i < m_arParagraphs.size(); ++i)
        {
            m_arParagraphs[i]->ToXml(oWriter);
        }
    }

    void CPage::WriteSectionToFile(bool bLastPage, NSStringUtils::CStringBuilder& oWriter)
    {
        // section
        int lWidthDx  = (int)(m_dWidth * c_dMMToDx);
        int lHeightDx = (int)(m_dHeight * c_dMMToDx);

        if (!bLastPage)
            oWriter.WriteString(L"<w:p><w:pPr><w:sectPr>");
        else
            oWriter.WriteString(L"<w:sectPr>");

        oWriter.WriteString(L"<w:pgSz w:w=\"");
        oWriter.AddInt((int)(m_dWidth * c_dMMToDx));
        oWriter.WriteString(L"\" w:h=\"");
        oWriter.AddInt((int)(m_dHeight * c_dMMToDx));
        oWriter.WriteString(L"\" w:orient=\"");
        (lWidthDx >= lHeightDx) ? oWriter.WriteString(L"landscape") : oWriter.WriteString(L"portrait");
        oWriter.WriteString(L"\"/>");

        if (!bLastPage)
            oWriter.WriteString(L"<w:pgMar w:top=\"0\" w:right=\"0\" w:bottom=\"0\" w:left=\"0\"/></w:sectPr><w:spacing w:line=\"1\" w:lineRule=\"exact\"/></w:pPr></w:p>");
        else
            oWriter.WriteString(L"<w:pgMar w:top=\"0\" w:right=\"0\" w:bottom=\"0\" w:left=\"0\" w:header=\"0\" w:footer=\"0\" w:gutter=\"0\"/></w:sectPr>");
    }
}
