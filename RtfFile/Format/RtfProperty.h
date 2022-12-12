﻿/*
 * (c) Copyright Ascensio System SIA 2010-2019
 *
 * This program is a free software product. You can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License (AGPL)
 * version 3 as published by the Free Software Foundation. In accordance with
 * Section 7(a) of the GNU AGPL its Section 15 shall be amended to the effect
 * that Ascensio System SIA expressly excludes the warranty of non-infringement
 * of any third-party rights.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR  PURPOSE. For
 * details, see the GNU AGPL at: http://www.gnu.org/licenses/agpl-3.0.html
 *
 * You can contact Ascensio System SIA at 20A-12 Ernesta Birznieka-Upisha
 * street, Riga, Latvia, EU, LV-1050.
 *
 * The  interactive user interfaces in modified source and object code versions
 * of the Program must display Appropriate Legal Notices, as required under
 * Section 5 of the GNU AGPL version 3.
 *
 * Pursuant to Section 7(b) of the License you must retain the original Product
 * logo when distributing the program. Pursuant to Section 7(e) we decline to
 * grant you any rights under trademark law for use of our trademarks.
 *
 * All the Product's GUI elements, including illustrations and icon sets, as
 * well as technical writing content are licensed under the terms of the
 * Creative Commons Attribution-ShareAlike 4.0 International. See the License
 * terms at http://creativecommons.org/licenses/by-sa/4.0/legalcode
 *
 */
#pragma once

#include "Basic.h"
#include "Utils.h"

#include "../../OOXML/Common/SimpleTypes_Drawing.h"

#if defined (_WIN32) || defined (_WIN64)
    #define MERGE_PROPERTY(propName, propObj)\
        if ( PROP_DEF != ##propObj##.##propName## )	propName = ##propObj##.##propName##;

    #define MERGE_PROPERTY_DEF(propName, propObj, defval)\
        if ( defval != ##propObj##.##propName## )	propName = ##propObj##.##propName##;
#else
    #define MERGE_PROPERTY(propName, propObj)\
        if ( PROP_DEF != propObj.propName )	propName = propObj.propName;

    #define MERGE_PROPERTY_DEF(propName, propObj, defval)\
        if ( defval != propObj.propName )	propName = propObj.propName;
#endif

#define DEFAULT_PROPERTY(propName)\
		propName= PROP_DEF;

#define DEFAULT_PROPERTY_DEF(propName, defval)\
		propName = defval;

#define RENDER_OOX_BOOL( prop, sResult, sName)\
		if ( 0 == prop )\
		{\
			sResult += L"<";\
			sResult += sName;\
            sResult += L" w:val=\"false\"/>";\
		}\
		else if (  PROP_DEF != prop  )\
		{\
			sResult += L"<";\
			sResult += sName;\
            sResult += L" w:val=\"true\"/>";\
		}

#define RENDER_OOX_INT( prop, sResult, sName)\
		if ( PROP_DEF != prop )\
		{\
			sResult += L"<";\
			sResult += sName;\
            sResult += L" w:val=\"" + std::to_wstring(prop) + L"\"/>";\
		}
#define RENDER_OOX_INT_ATTRIBUTE( prop, sResult, sName)\
		if ( PROP_DEF != prop )\
		{\
			sResult += L" ";\
			sResult += sName;\
            sResult += L"=\"" + std::to_wstring(prop) + L"\"";\
		}
#define RENDER_OOX_BOOL_ATTRIBUTE( prop, sResult, sName)\
		if ( 0 == prop )\
		{\
			sResult += L" ";\
			sResult += sName;\
			sResult += L"=\"0\"";\
		}\
		else if (  PROP_DEF != prop  )\
		{\
			sResult += L" ";\
			sResult += sName;\
			sResult += L"=\"1\"";\
		}

#define RENDER_RTF_BOOL( prop, sResult, sName)\
		if ( 0 == prop )\
		{\
			sResult += L"\\";\
			sResult += sName;\
			sResult += L"0";\
		}\
		else if (  PROP_DEF != prop  )\
		{\
			sResult += L"\\";\
			sResult += sName;\
		}

#define RENDER_RTF_INT( prop, sResult, sName)\
		if ( PROP_DEF != prop )\
		{\
			sResult += L"\\";\
			sResult += sName;\
            sResult += std::to_wstring( prop  );\
		}
#define RENDER_RTF_SHAPE_PROP( sName, sResult, prop)\
        if ( PROP_DEF != prop )\
            sResult += L"{\\sp{\\sn " + std::wstring(sName) + L"}{\\sv " + std::to_wstring( prop ) + L"}}";

class RtfSection;
typedef boost::shared_ptr<RtfSection> RtfSectionPtr;

class RtfFont : public IRenderableProperty
{
public: 
	typedef enum {ft_none,ft_flomajor , ft_fhimajor ,ft_fdbmajor ,ft_fbimajor ,ft_flominor ,ft_fhiminor ,ft_fdbminor ,ft_fbiminor}FontTheme;
		
	typedef enum {ff_none, ff_fnil ,ff_froman ,ff_fswiss ,ff_fmodern ,ff_fscript ,ff_fdecor ,ff_ftech ,ff_fbidi} FontFamily;
	
    FontTheme       m_eFontTheme;
    FontFamily      m_eFontFamily;
    std::wstring    m_sPanose;
    int             m_nID;
    std::wstring	m_sName;
    std::wstring	m_sAltName;
    int             m_nCharset;
    int             m_nCodePage;
    int             m_nPitch;

	RtfFont();

	int GetType();
	bool operator==( const RtfFont& oFont);
	bool IsValid();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfColor : public IRenderableProperty
{
public: 
	enum _ThemeColor {TC_NONE,cmaindarkone ,cmainlightone ,cmaindarktwo ,cmainlighttwo ,caccentone ,caccenttwo ,caccentthree ,caccentfour ,caccentfive ,caccentsix ,chyperlink ,cfollowedhyperlink ,cbackgroundone ,ctextone ,cbackgroundtwo ,ctexttwo};
	
	bool		m_bAuto;

    BYTE		m_byteRed;
    BYTE		m_byteGreen;
    BYTE		m_byteBlue;
//for rtf read only
	BYTE		m_byteShade;
	BYTE		m_byteTint;
	_ThemeColor	m_eTheme;

	int			m_index;

//--------------------------------------------------------
	RtfColor();
	RtfColor(int nHex);
	RtfColor(BYTE r, BYTE g, BYTE b);
	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetAuto();
	void SetDefault();
	BYTE GetR();
	BYTE GetG();
	BYTE GetB();
	
	bool operator==(const RtfColor& oColor);
	const RtfColor& operator=( const RtfColor& oColor );

	void SetHEX(int color);
	int GetRGB()const;
	void SetRGB(unsigned int color);
	void SetRGB(BYTE red, BYTE green, BYTE blue);
	void GetHSL(double &dH, double &dS,double &dL);
	void SetHSL(double dH, double dS,double dL);
	void SetRGBPercent(int nRedPer, int nGreenPer, int nBluePer);
	void SetHEXString(std::wstring hex);
	std::wstring ToHexColor(bool bBGR = false);
	
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);

	BYTE SetShade(BYTE bShade);
	BYTE SetTint(BYTE bTint);

	static bool GetHighlightByColor( RtfColor oOutputColor,std::wstring& oStr ); //todo
	static RtfColor GetColorByPreset( std::wstring oStr );
	static std::wstring GetPresetByColor( RtfColor oCol ); //стр. 3320
	static bool GetThemeByString( std::wstring sTheme, _ThemeColor & oOutTheme );
	static bool GetThemeByOOX( SimpleTypes::EShemeColorVal val, _ThemeColor & oOutTheme );
	static bool GetStringByTheme( std::wstring sTheme , _ThemeColor& oOutTheme );

	std::wstring GetHighLight();

private:
	void RGB2HSL(unsigned char unR, unsigned char unG, unsigned char unB, double& dH, double& dS, double& dL);
	void HSL2RGB(double dH, double dS, double dL, unsigned char &unR, unsigned char& unG, unsigned char& unB);
	double Hue2RGB(double dV1, double dV2, double dH);

	const double c_d_1_6 = 1.0 / 6.0;
	const double c_d_1_3 = 1.0 / 3.0;
	const double c_d_2_3 = 2.0 / 3.0;
				
	std::wstring WriteOOXAttribute( std::wstring sParam );
};

class RtfShading
{
public: 
	enum _ShadingType
	{	st_none, 
		st_chbghoriz,	//bghoriz	Specifies a horizontal background pattern for the paragraph.
		st_chbgvert,	//bgvert	Specifies a vertical background pattern for the paragraph.
		st_chbgfdiag,	//bgfdiag	Specifies a forward diagonal background pattern for the paragraph (\\\\).
		st_chbgbdiag,	//bgbdiag	Specifies a backward diagonal background pattern for the paragraph (////).
		st_chbgcross,	//bgcross	Specifies a cross background pattern for the paragraph.
		st_chbgdcross,	//bgdcross	Specifies a diagonal cross background pattern for the paragraph.
		st_chbgdkhoriz,	//bgdkhoriz	Specifies a dark horizontal background pattern for the paragraph.
		st_chbgdkvert,	//bgdkvert	Specifies a dark vertical background pattern for the paragraph.
		st_chbgdkfdiag,	//bgdkfdiag	Specifies a dark forward diagonal background pattern for the paragraph (\\\\).
		st_chbgdkbdiag,	//bgdkbdiag	Specifies a dark backward diagonal background pattern for the paragraph (////).
		st_chbgdkcross,	//bgdkcross	Specifies a dark cross background pattern for the paragraph.
		st_chbgdkdcross,	//bgdkdcross	Specifies a dark diagonal cross background pattern for the paragraph.
		st_clshdrawnil,	//clshdrawnil	Specifies a dark diagonal cross background pattern for the paragraph.
	}	m_eType;

	int m_nValue;
	int m_nForeColor;
	int m_nBackColor;

	RtfShading();

	bool operator==( const RtfShading& oChar );
	bool IsValid();
	int GetType();

	void SetDefaultRtf( );
	void SetDefaultOOX( );
	void SetDefault( );

	void Merge( RtfShading& oParPr );
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfShadingPar : public RtfShading
{
public: 
	int GetType();
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
};

class RtfShadingChar : public RtfShading
{
public: 
	int GetType();
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
};

class RtfShadingCell : public RtfShading
{
public: 
	int GetType();
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
};

class RtfShadingRow : public RtfShading
{
public:
	int GetType();
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
};

class RtfShadingTableStyle : public RtfShading
{
public: 
	int GetType();
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
};

class RtfBorder
{
public: 
	enum _BorderType
	{ 
		bt_none,
		bt_brdrs,			//brdrs	Single-thickness border.
		bt_brdrth,			//brdrth	Double-thickness border.
		bt_brdrsh,			//brdrsh	Shadowed border.
		bt_brdrdb,			//brdrdb	Double border.
		bt_brdrdot,			//brdrdot	Dotted border.
		bt_brdrdash,		//brdrdash	Dashed border.
		bt_brdrhair,		//brdrhair	Hairline border.
		bt_brdrdashsm,		//brdrdashsm	Dashed border (small).
		bt_brdrdashd,		//brdrdashd	Dot-dashed border.
		bt_brdrdashdd,		//brdrdashdd	Dot-dot-dashed border.
		bt_brdrdashdot,		//brdrdashdot	Dot-dashed border (alias for \brdrdashd read but not written by Word)
		bt_brdrinset,		//brdrinset	Inset border.
		bt_brdrnone,		//brdrnone	No border.
		bt_brdroutset,		//brdroutset	Outset border.
		bt_brdrtriple,		//brdrtriple	Triple border.
		bt_brdrtnthsg,		//brdrtnthsg	Thick-thin border (small).
		bt_brdrthtnsg,		//brdrthtnsg	Thin-thick border (small).
		bt_brdrtnthtnsg,	//brdrtnthtnsg	Thin-thick thin border (small).
		bt_brdrtnthmg,		//brdrtnthmg	Thick-thin border (medium).
		bt_brdrthtnmg,		//brdrthtnmg	Thin-thick border (medium).
		bt_brdrtnthtnmg,	//brdrtnthtnmg	Thin-thick thin border (medium).
		bt_brdrtnthlg,		//brdrtnthlg	Thick-thin border (large).
		bt_brdrthtnlg,		//brdrthtnlg	Thin-thick border (large).
		bt_brdrtnthtnlg,	//brdrtnthtnlg	Thin-thick-thin border (large).
		bt_brdrwavy,		//brdrwavy	Wavy border.
		bt_brdrwavydb,		//brdrwavydb	Double wavy border.
		bt_brdrdashdotstr,	//brdrdashdotstr	Striped border.
		bt_brdremboss,		//brdremboss	Embossed border.
		bt_brdrengrave,		//brdrengrave	Engraved border.
	}	m_eType;
	
	int m_nWidth;
	int m_nSpace;
	int m_nColor;

	RtfBorder();
	bool operator==( const RtfBorder& oChar );

	bool IsValid();
	int GetType();

	void SetDefaultRtf( );
	void SetDefaultOOX( );
	void SetDefault( );
	void SetEmpty( );

	void Merge( RtfBorder& oBorPr );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
	
	static bool GetStringRtfByType( _BorderType nValue, std::wstring& sValue );
	static std::wstring GetStringOOXByType( _BorderType nValue, std::wstring& sValue );
};

class RtfTab : public IRenderableProperty
{
public:
	enum _TabLeader
	{	tl_none,
		tl_dot ,	//tldot 	Absolute position tab with a leading that uses period symbols (.....).
		tl_mdot ,	//tlmdot 	Absolute position tab with a leading that uses middle dot symbols (•••••).
		tl_hyph ,	//tlhyph 	Absolute position tab with a leading that uses minus symbols (-----).
		tl_ul ,		//tlul 		Absolute position tab with a leading that uses underscore symbols (_____).
	}m_eLeader;

	enum _TabKind
	{	tk_none,
		tk_tql,
		tk_tqr,		//tqr	Flush-right tab.
		tk_tqc,		//tqc	Centered tab.
		tk_tqdec,	//tqdec	Decimal tab.
		tk_tqbar,	//tbN	Bar tab position in twips from the left margin.
		tk_tqclear
	}	m_eKind;
	
	int m_nTab;		//tbN or \txN	Tab position in twips from the left margin.

	RtfTab();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfTabs: public IRenderableProperty
{
public: 
	std::vector<RtfTab> m_aTabs;

	RtfTabs();
	RtfTabs( const RtfTabs& oTabs );

	const RtfTabs& operator=( const RtfTabs& oTabs );

	void Merge( const RtfTabs& oTabs );
	void SetDefault();

	std::wstring RenderToRtf(RenderParameter oRenderParameter);
	std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

//----------------------------------------------------------------------------------------------------------

class RtfCharProperty;
typedef boost::shared_ptr<RtfCharProperty> RtfCharPropertyPtr;

class RtfCharProperty: public IRenderableProperty
{
public: 
	bool m_bListLevel;

	enum _UnderlineStyle {uls_none, uls_Single, uls_Dotted,uls_Dashed ,uls_Dash_dotted,uls_Dash_dot_dotted,uls_Double,uls_Heavy_wave,uls_Long_dashe,uls_Stops_all,uls_Thick,uls_Thick_dotted,uls_Thick_dashed,uls_Thick_dash_dotted,uls_Thick_dash_dot_dotted,uls_Thick_long_dashed,uls_Double_wave,uls_Word,uls_Wave};

	int m_nAnimated;		//animtextN	Animated text properties (note: Word 2007 ignores this control word):

	int m_bBold;			//b*	Bold.
	int m_bCaps;			//caps*	All capitals.
	int m_nScalex;			//charscalexN 	Character scaling value. The N argument is a value representing a percentage (default is 100).
	int m_nCharStyle;		//csN	Designates character style. If a character style is specified, style properties must be specified with the character run. N refers to an entry in the style table.
	int m_nDown;			//dnN	Move down N half-points (default is 6).
	int m_bEmbo;			//embo*	Emboss.
	int m_nCharacterSpacing;//expndtwN	Expansion or compression of the space between characters in twips; a negative value compresses. For backward compatibility, both \expndtwN and \expndN should be emitted.		
	int m_nFitText;			//fittextN	Fit the text in the current group in N twips. When N is set to -1 (\fittext-1), it indicates a continuation of the previous \fittextN run. In other words, {\fittext1000 Fit this} {\fittext-1 text} fits the string “Fit this text” in 1000 twips.
	int m_nFont;			//fN	Font number. N refers to an entry in the font table.
	int m_nFont2; 
	int m_nFont3; 
	int m_nFontSize;		//fsN	Font size in half-points (default is 24).
	int m_bItalic;			//i*	Italic.
	int m_bImprint;			//impr*	Engrave (imprint).
	int m_nKerning;			//kerningN	Point size (in half-points) above which to kern character pairs. \kerning0 turns off kerning.
	int m_bRightToLeft;		//rtlch	Character data following this control word is treated as a right-to-left run.
	int m_nLanguage;
	int m_nLanguageAsian;	// for East Asian text
	int m_nComplexScript;	// 1 - means South East Asian complex script; 0 - means not South East Asian script
	int m_bOutline;			//outl*	Outline.
	int m_bScaps;			//scaps*	Small capitals.
	int m_bShadow;			//shad*	Shadow.
	int m_bStrike;			//strike*	Strikethrough.
	int m_nStriked;			//striked1	Double strikethrough. \striked0 turns it off.
	int m_bSub;				//sub	Subscripts text and shrinks point size according to font information.
	int m_bSuper;			//super	Superscripts text and shrinks point size according to font information.
	int m_bHidden;			//v*	Hidden text.
	int m_nHightlited; 

	int				m_nForeColor; 

	int				m_nCrAuth;
	int				m_nCrDate;

	int				m_nDeleted;
	int				m_nRevised;
	int				m_nRevauth;
	int				m_nRevdttm;
	int				m_nRevauthDel;
	int				m_nRevdttmDel;

	int				m_nInsrsid;

//	int				m_bUnderline;					//ul*	Continuous underline. \ul0 turns off all underlining.
	_UnderlineStyle	m_eUnderStyle;		//
	int				m_nUnderlineColor;	//
	int				m_nUp;				//upN	Move up N half-points (default is 6).

	RtfCharPropertyPtr	m_pOldCharProp;
	RtfBorder			m_poBorder;
	RtfShadingChar		m_poShading;
	
	bool operator==( const RtfCharProperty& oChar );

	RtfCharProperty();
	int GetType();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

	void Merge( RtfCharProperty& oCharPr, bool bAll = true );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfListLevelProperty : IRenderableProperty
{
public: 
	int		m_nLevel;			///OOX
	int		m_nNumberType;		//levelnfcN \levelnfcnN Specifies the number type for the level
	int		m_bTentative;		//lvltentative 	Specifies that a given numbering level was been saved by a producer but was not used in the parent document. This means that this numbering level may be redefined by a future consumer without changing the actual content of the document.
	int		m_nJustification;	//leveljcN \leveljcnN	0	Left justified 1	Center justified 2	Right justified
	int		m_nFollow;			//levelfollowN
	int		m_nStart;			//levelstartatN	N specifies the start-at value for the level.
    std::wstring m_sText;			//как в rtf текст, но сдвинут от нуля на 1 // \'03\'00.\'01 -> ("%d%d%d",4,1,2)
    std::wstring m_sNumber;
	int		m_nNoRestart;		//levelnorestartN	1 if this level does not restart its count each time a super ordinate level is incremented; 0 if this level does restart its count each time a super ordinate level is incremented.
	int		m_nLegal;			//levellegalN	1 if any list numbers from previous levels should be converted to Arabic numbers; 0 if they should be left with the format specified by their own level’s definition.
	int		m_nPictureIndex;	//levelpictureN	Determines which picture bullet from the \listpicture destination should be applied.

	RtfTabs m_oTabs;			//ParagraphProp
	int		m_nFirstIndent;
	int		m_nIndent;
	int		m_nIndentStart;
	int		m_nSpace;
	
	RtfCharProperty m_oCharProp; //Char

	RtfListLevelProperty();

	bool IsValid();
	std::wstring GenerateListText();
	void SetDefault();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
    std::wstring RenderToOOX2(RenderParameter oRenderParameter, int lvl = PROP_DEF);

	static std::wstring GetFormat( int nNumFormat);
	static int GetFormat( std::wstring sFormat);

    std::wstring GetLevelTextOOX();
    void SetLevelTextOOX(std::wstring sText);
};

class RtfListProperty : public IRenderableProperty, public ItemContainer<RtfListLevelProperty>
{
public: 
	int		m_nID;		//listidN	Each list must have a unique list ID that should be randomly generated. N is a long integer. The list ID cannot be between –1 and –5. 
	int		m_nTemplateId;		//listtemplateidN	Each list should have a unique template ID as well, which also should be randomly generated. The template ID –1 means the template ID is undefined. N is a long integer. 
	int		m_nListSimple;		//listsimpleN	1 if the list has one level; 0 (default) if the list has nine levels.
	int		m_bListHybrid;		//listhybrid	Present if the list has 9 levels, each of which is the equivalent of a simple list. Only one of \listsimpleN and \listhybrid should be present. Word 2000 and newer versions will write lists with the \listhybrid property.
    std::wstring m_sName;		//listname	The argument for \listname is a string that is the name of this list. Names allow ListNum fields to specify the list to which they belong. This is a destination control word.

	RtfListProperty();

	void SetDefault();
	bool IsValid();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfListOverrideProperty : IRenderableProperty
{
public: 
	class ListOverrideLevels : IRenderableProperty
	{
	public: 
		class ListOverrideLevel
		{
			public:
				int m_nLevelIndex;	//listoverrideformat	Number of list format override levels within this list override (should be either 1, 9, or missing, which means 0).
				int m_nStart;		//listoverridestartat	Indicates an override of the start-at value.
				RtfListLevelProperty m_oLevel;
				ListOverrideLevel()
				{
					m_nLevelIndex = PROP_DEF;
					m_nStart = PROP_DEF;
				}
		};
		std::vector<ListOverrideLevel> m_aOverrideLevels;

		ListOverrideLevels();
		ListOverrideLevels( const ListOverrideLevels& oOverLevel );
		ListOverrideLevels& operator=( const ListOverrideLevels& oOverLevel );

		void SetDefault();

		std::wstring RenderToRtf(RenderParameter oRenderParameter);
		std::wstring RenderToOOX(RenderParameter oRenderParameter);
	};

	int m_nListID;		//listidN	Should exactly match the \listid of one of the lists in the List table. The value N is a long integer.
	int m_nIndex;		//lsN	The (1-based) index of this \listoverride in the \listoverride table. This value should never be zero inside a \listoverride and must be unique for all \listoverride’s within a document. The valid values are from 1 to 2000. The value 0 means no list.
	
	ListOverrideLevels m_oOverrideLevels;

	RtfListOverrideProperty();

	bool IsValid();
	void SetDefault();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfStyle;
typedef boost::shared_ptr<RtfStyle> RtfStylePtr;

class RtfStyle: public IRenderableProperty
{
public: 
	enum _StyleType { st_none, stParagraph, stCharacter, stSection, stTable, stNumbering} ;

	_StyleType		m_eType;
    std::wstring	m_sName;
	int				m_nID;
    std::wstring	m_sID;

	int m_bAdditive;
	int m_nBasedOn;
	int m_nNext;
	int m_bHidden;
	int m_nLink;
	int m_bLocked;
	int m_bPersonal;
	int m_bCompose;
	int m_bReply;
	int m_nSemiHidden;
	int m_bQFormat;
	int m_nPriority;
	int m_bUnhiddenWhenUse;

	RtfStyle();

	bool IsValid();
	int GetType();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

	virtual void Merge( RtfStylePtr oStyle );

	bool operator==( const RtfStyle& oProperty );
	std::wstring RenderToRtfBegin( RenderParameter oRenderParameter );

    std::wstring RenderToRtfEnd( RenderParameter oRenderParameter );
    std::wstring RenderToOOXBegin(RenderParameter oRenderParameter);
    std::wstring RenderToOOXEnd(RenderParameter oRenderParameter);

	std::wstring RenderToRtf(RenderParameter oRenderParameter);
	std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfTableProperty: public IRenderableProperty
{
public: 
	enum _HRef
	{
		hr_none,
		hr_phmrg,	// tphmrg	Use margin as horizontal reference frame.
		hr_phpg,	//tphpg	Use page as horizontal reference frame.
		hr_phcol	//tphcol	Use column as horizontal reference frame. This is the default if no horizontal table positioning information is given.
	} ;
	enum _VRef
	{
		vr_none,
		vr_pvmrg,	//tpvmrg	Position table vertically relative to the top margin. This is the default if no vertical table positioning information is given.
		vr_pvpg,	//tpvpg	Position table vertically relative to the top of the page.
		vr_pvpara	//tpvpara	Position table vertically relative to the upper left corner of the next unframed paragraph in the stream.
	};
	enum _HPos
	{
		hp_none, 
		hp_posxc,	//tposxc	Center table within the horizontal reference frame.
		hp_posxi,	//tposxi	Position table inside the horizontal reference frame.
		hp_posxo,	//tposxo	Position table outside the horizontal reference frame.
		hp_posxl,	//tposxl	Position table at the left of the horizontal reference frame.
		hp_posxr	//tposxr	Position table at the right of the horizontal reference frame.
	};
	enum _VPos
	{
		vp_none,
		vp_posyt,	//tposyt	Position table at the top of the vertical reference frame.
		vp_posyil,	//tposyil	Position table to be inline.
		vp_posyb,	//tposyb	Position table at the bottom of the vertical reference frame.
		vp_posyc,	//tposyc	Center table within the vertical reference frame
		vp_posyin,	//tposyin	Position table inside within the vertical reference frame.
		vp_posyout	//tposyout	Position table outside within the vertical reference frame.
	};

	int m_bBidi;			//taprtl	Table direction is right to left.
	int m_nAutoFit;			//trautofitN	AutoFit:0	No AutoFit (default).1	AutoFit is on for the row. Overridden by \clwWidthN and \trwWidthN in any table row
	int m_nGraph;			//trgaphN	Half the space between the cells of a table row in twips.

	int nTableIndent;		//tblindN 
	int eTableIndentUnit;	// tblindtypeN  

	enum _RowJust
	{
		rj_none,
		rj_trql,		//trql	Left-justifies a table row with respect to its containing column.
		rj_trqr,		//trqr	Right-justifies a table row with respect to its containing column.
		rj_trqc			//trqc	Centers a table row with respect to its containing column.
	}	m_eJust; 

	int m_nWrapLeft;		//tdfrmtxtLeftN	Distance in twips, between the left of the table and surrounding text (default is 0).
	int m_nWrapRight;		//tdfrmtxtRightN	Distance in twips, between the right of the table and surrounding text (default is 0).
	int m_nWrapTop;			//tdfrmtxtTopN	Distance in twips, between the top of the table and surrounding text (default is 0).
	int m_nWrapBottom;		//tdfrmtxtBottomN	Distance in twips, between the bottom of the table and surrounding text (default is 0).
	int m_bOverlap;			//tabsnoovrlp	Do not allow table to overlap with other tables or shapes with similar wrapping not contained within it.

	_HRef			m_eHRef;
	_VRef			m_eVRef;
	_HPos			m_eHPos;
	int				m_nHPos;				//tposxN	Position table N twips from the left edge of the horizontal reference frame.
	_VPos			m_eVPos;
	int				m_nVPos;				//tposyN	Position table N twips from the top edge of the vertical reference frame.

	int				m_nLeft;
	
	int				m_nWidth;				//trwWidthN	Width of invisible cell at the end of the row. Used only when rows have different widths.
	int				m_eWidthUnit;			//trftsWidthN	Units for \trwWidthN:

	int m_nDefCellMarBottom;	//trpaddbN	Default bottom cell margin or padding for the row.
	int m_nDefCellMarLeft;		//trpaddlN	Default left cell margin or padding for the row.
	int m_nDefCellMarRight;		//trpaddrN	Default right cell margin or padding for the row.
	int m_nDefCellMarTop;		//trpaddtN	Default top cell margin or padding for the row.

	int m_eDefCellMarBottomUnit; //Units for \trpaddbN: 0	Null. Ignore \trpaddbN in favor of \trgaphN (Word 97 style padding). 3	Twips.
	int m_eDefCellMarLeftUnit;
	int m_eDefCellMarRightUnit;
	int m_eDefCellMarTopUnit;

	int m_nDefCellSpBottom;		//trspdbN	Default bottom cell spacing for the row. The total vertical spacing between adjacent cells is equal to the sum of \trspdtN from the bottom cell and \trspdbN from the top cell, both of which will have the same value when written by Word.
	int m_nDefCellSpLeft;		//trspdlN
	int m_nDefCellSpRight;		//trspdrN
	int m_nDefCellSpTop;		//trspdtN

	int m_eDefCellSpBottomUnit; //trspdfbN Units for \trspdbN: 0	Null. Ignore \trspdbN. 3	Twips.
	int m_eDefCellSpLeftUnit;
	int m_eDefCellSpRightUnit;
	int m_eDefCellSpTopUnit;

	RtfBorder m_oBorderLeft;
	RtfBorder m_oBorderRight;
	RtfBorder m_oBorderTop;
	RtfBorder m_oBorderBottom;
	RtfBorder m_oBorderVert;
	RtfBorder m_oBorderHor;

	RtfShadingRow m_oShading;
	int m_nStyle;

//Autoformatting Flags
	int m_bAutoFirstRow;	//tbllkhdrrows	Flag sets table autoformat to format the first (header) row.
	int m_bAutoLastRow;		//tbllklastrow	Flag sets table autoformat to format the last row.
	int m_bAutoFirstCol;	//tbllkhdrcols	Flag sets table autoformat to format the first (header) column.
	int m_bAutoLastCol;		//tbllklastcol	Flag sets table autoformat to format the last column.
	int m_bAutoNoRowBand;	//tbllknorowband	Specifies row banding conditional formatting shall not be applied.
	int m_bAutoNoColBand;	//tbllknocolband	Specifies column banding conditional formatting shall not be applied.

	int m_nRowBandSize;		//tscbandshN	Count of rows in a row band
	int m_nColBandSize;		//tscbandsvN	Count of cells in a cell band

	RtfTableProperty();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

	bool IsValid();
	void Merge( RtfTableProperty& oTablePr );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfFrame : public IRenderableProperty
{
public:
	enum _HRef
	{
		hr_none,
		hr_phmrg,
		hr_phpg,
		hr_phcol
	};
	enum _VRef
	{
		vr_none,
		vr_pvmrg,
		vr_pvpg,
		vr_pvpara
	};
	enum _HPos
	{
		hp_none, 
		hp_posxc,
		hp_posxi,
		hp_posxo,
		hp_posxl,
		hp_posxr 
	};
	enum _VPos
	{
		vp_none,
		vp_posyt,
		vp_posyil,
		vp_posyb,
		vp_posyc,
		vp_posyin,
		vp_posyout
	};
	enum _TextWrap
	{
		tw_none,
		tw_wrapdefault,
		tw_wraparound,
		tw_wraptight,
		tw_wrapthrough
	};


	int			m_nWidth;
	int			m_nHeight;
	_HRef		m_eHRef;
	_VRef		m_eVRef;
	_HPos		m_eHPos;
	int			m_nHPos;
	_VPos		m_eVPos;
	int			m_nVPos;
	int			m_bLockAnchor;
	_TextWrap	m_eWrap;
	int			m_DropcapType;
	int			m_DropcapLines;
	int			m_nHorSpace;		// dxfrtextN	Distance in twips of a positioned paragraph from text in the main text flow in all directions.
	int			m_nVerSpace;		// dfrmtxtxN	N is the horizontal distance in twips from text on both sides of the frame.
	int			m_nAllSpace;		// dfrmtxtyN	N is the vertical distance in twips from text on both sides of the frame.

	RtfFrame();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

	void Merge( RtfFrame& oFramePr );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
	
	void ApplyParagraphProp( RtfTableProperty& oProp );
};

class RtfCellProperty;
typedef boost::shared_ptr<RtfCellProperty>	RtfCellPropertyPtr;

class RtfCellProperty: public IRenderableProperty
{	
public:
	int m_bMergeFirst;			//clmgf		The first cell in a range of table cells to be merged.
	int m_bMerge;				//clmrg		Contents of the table cell are merged with those of the preceding cell.
	int m_bMergeFirstVertical;	//clvmgf	The first cell in a range of table cells to be vertically merged.
	int m_bMergeVertical;		//clvmrg	Contents of the table cell are vertically merged with those of the preceding cell.

	int m_bFitText;				//clFitText	Fit text in cell, compressing each paragraph to the width of the cell.
	int m_bNoWrap;				//clNoWrap	Do not wrap text for the cell. Only has an effect if the table cell does not have a preferred \clwWidthN, which overrides \trautofitN.

	int m_nPaddingLeft;			//clpadlN	Left cell margin or padding. Overrides \trpaddlN.
	int m_ePaddingLeftUnit;		//clpadflN	Units for \clpadlN:
	int m_nPaddingRight;
	int m_ePaddingRightUnit;	
	int m_nPaddingTop;
	int m_ePaddingTopUnit;	
	int m_nPaddingBottom;
	int m_ePaddingBottomUnit;	

	int m_nSpacingLeft;			//clsplN	Left cell margin or padding. Overrides \trspdlN.
	int m_eSpacingLeftUnit;		//clspflN	Units for \clsplN:
	int m_nSpacingRight;
	int m_eSpacingRightUnit;
	int m_nSpacingTop;
	int m_eSpacingTopUnit;
	int m_nSpacingBottom;
	int m_eSpacingBottomUnit;

	int				m_nWidth;			//clwWidthN	Preferred cell width. Overrides \trautofitN.
	int				m_eWidthUnit;		//clftsWidthN	Units for \clwWidthN:

	int				m_bHideMark;		//clhidemark	This control word specifies whether the end of cell glyph shall influence the height of the given table row in the table. If it is specified, then only printing characters in this cell shall be used to determine the row height.

	RtfBorder m_oBorderDiagonalLR;
	RtfBorder m_oBorderDiagonalRL;
	RtfBorder m_oBorderLeft;
	RtfBorder m_oBorderTop;
	RtfBorder m_oBorderRight;
	RtfBorder m_oBorderBottom;
	RtfBorder m_oBorderInsideH;
	RtfBorder m_oBorderInsideV;

	RtfShadingCell	m_oShading;
	int				m_nShadingPctFrom;

	typedef enum{ ca_none, 
					ca_Top,	//clvertalt	Text is top-aligned in cell (the default).
					ca_Center,	//clvertalc	Text is centered vertically in cell.
					ca_Bottom //clvertalb	Text is bottom-aligned in cell.
				} CellAlign;
	int m_eAlign;
	typedef enum{ cf_none,
					cf_lrtb,	//cltxlrtb	Text in a cell flows from left to right and top to bottom (default).
					cf_tbrl,	//cltxtbrl	Text in a cell flows right to left and top to bottom.
					cf_btlr,	//cltxbtlr	Text in a cell flows left to right and bottom to top.
					cf_lrtbv,	//cltxlrtbv	Text in a cell flows left to right and top to bottom, vertical.
					cf_tbrlv//cltxtbrlv	Text in a cell flows top to bottom and right to left, vertical.
				} CellFlow;
	CellFlow m_oCellFlow;

	int m_nCellx;		//cellxN	Defines the right boundary of a table cell, including its half of the space between cells.
	int m_nSpan;

//Table Style Specific
	int m_bStyleFirstRow;		// tscfirstrow	This cell is in the first row.
	int m_bStyleLastRow;		// tsclastrow	This cell is in the last row.
	int m_bStyleFirstCol;		// tscfirstcol	This cell is in the first column.
	int m_bStyleLastCol;		// tsclastcol	This cell is in the last column.
	int m_bStyleOddRowBand;		// tscbandhorzodd	This cell is in the odd row band.
	int m_bStyleEvenRowBand;		// tscbandhorzeven	This cell is in the even row band.
	int m_bStyleOddColBand;		// tscbandvertodd	This cell is in the odd column band.
	int m_bStyleEvenColBand;		// tscbandverteven	This cell is in the even column band.
	int m_bStyleNWCell;		// tscnwcell	This is the NW (north west) cell in the table (upper left).
	int m_bStyleNECell;		// tscnecell	NE cell
	int m_bStyleSWCell;		// tscswcell	SW cell.
	int m_bStyleSECell;		// tscsecell	SE cell.

	RtfCellProperty();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

	bool IsValid();
	void Merge( RtfCellProperty& oCellPr );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);

	bool GetAlignFromStringRtf(  std::wstring & oAlign, CellAlign& oOutput );
	bool GetAlignFromStringOOX(  std::wstring & oAlign, CellAlign& oOutput );
};

class RtfRowProperty;
typedef boost::shared_ptr<RtfRowProperty>	RtfRowPropertyPtr;

class RtfRowProperty: public RtfTableProperty, public ItemContainer< RtfCellProperty >
{
public: 
	int m_nRightToLeft; //bi direction 
	int m_nIndex;		//irowN	N is the row index of this row.
	int m_nBandIndex;	//irowbandN	N is the row index of the row, adjusted to account for header rows. A header row has a value of –1.

	int m_bLastRow;		//lastrow	Output if this is the last row in the table.

	int m_bIsHeader;	//trhdr	Table row header. This row should appear at the top of every page on which the current table appears.
	int m_bKeep;		//trkeep	Keep table row together. This row cannot be split by a page break. This property is assumed to be off unless the control word is present.

	int m_nHeight;		//trrhN	Height of a table row in twips. When 0, the height is sufficient for all the text in the line; when positive, the height is guaranteed to be at least the specified height; when negative, the absolute value of the height is used, regardless of the height of the text in the line.

	int	m_nWidthStartInvCell;		//trwWidthBN	Width of invisible cell at the beginning of the row. Used only in cases where rows have different widths.
	int	m_eWidthStartInvCellUnit;	//trftsWidthBN	Units for \trwWidthBN:

	int	m_nWidthEndInvCell;			//trwWidthAN	Width of invisible cell at the end of the row. Used only when rows have different widths.
	int	m_eWidthEndInvCellUnit;		//trftsWidthAN	Units for \trwWidthAN:

	int m_nGridBefore;	//для oox
	int m_nGridAfter;	//для oox

	int m_bStyleFirstRow;		// tscfirstrow	This cell is in the first row.
	int m_bStyleLastRow;		// tsclastrow	This cell is in the last row.
	int m_bStyleFirstCol;		// tscfirstcol	This cell is in the first column.
	int m_bStyleLastCol;		// tsclastcol	This cell is in the last column.
	int m_bStyleOddRowBand;		// tscbandhorzodd	This cell is in the odd row band.
	int m_bStyleEvenRowBand;	// tscbandhorzeven	This cell is in the even row band.
	int m_bStyleOddColBand;		// tscbandvertodd	This cell is in the odd column band.
	int m_bStyleEvenColBand;	// tscbandverteven	This cell is in the even column band.
	int m_bStyleNWCell;			// tscnwcell	This is the NW (north west) cell in the table (upper left).
	int m_bStyleNECell;			// tscnecell	NE cell
	int m_bStyleSWCell;			// tscswcell	SW cell.
	int m_bStyleSECell;			// tscsecell	SE cell.

	int m_nTrAuth;
	int m_nTrDate;

	RtfRowPropertyPtr	m_pOldRowProperty;

	RtfRowProperty();

	void SetDefaultRtf();
	void SetDefaultOOX();
	//5.5 Доверенность_MO_Q139.rtf
	void SetDefault();

	bool IsValid();
	void Merge( RtfRowProperty& oRowPr );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

//----------------------------------------------------------------------------------------------------------
class RtfParagraphProperty;
typedef boost::shared_ptr<RtfParagraphProperty>		RtfParagraphPropertyPtr;

class RtfParagraphProperty: public IRenderableProperty
{
public: 
	bool					m_bHidden;
	bool					m_bOldList;
	RtfParagraphPropertyPtr m_pOldParagraphProp;
//-------------
	int		m_bAutoHyphenation;	//hyphpar	Switches automatic hyphenation for the paragraph. Append 1 or nothing to toggle property on; append 0 to turn it off.
	int		m_bInTable;			//intbl	Paragraph is part of a table.
	int		m_nItap;			//itapN	Paragraph nesting level, where 0 is the main document, 1 is a table cell, 2 is a nested table cell, 3 is a doubly nested table cell, and so forth (default is 1).
	int		m_bKeep;			//keep	Keep paragraph intact (completely on one page if possible).
	int		m_bKeepNext;		//keepn	Keep paragraph with the next paragraph.
	int		m_bPageBB;			//pagebb	Break page before the paragraph.
	int		m_nOutlinelevel;	// outlinelevelN	Outline level of paragraph. The N argument is a value from 0 to 8 representing the outline level of the paragraph. In the default case, no outline level is specified (same as body text).
	int		m_nStyle;			//sN	Designates paragraph style. If a paragraph style is specified, style properties must be specified with the paragraph. N references an entry in the style sheet.

	typedef enum 
	{
		pa_none,
		pa_qc,					//qc	Centered.
		pa_qj,					//qj	Justified.
		pa_ql,					//ql	Left-aligned (the default).
		pa_qr,					//qr	Right-aligned.
		pa_qd,					//qd	Distributed.
		pa_qk0,					//qkN	Percentage of line occupied by Kashida justification (0 – low, 10 – medium, 20 – high).
		pa_qk10,
		pa_qk20,
	} ParagraphAlign;
	
	int m_eAlign;
	
	typedef enum 
	{
		fa_none,
		fa_faauto,				//faauto	Font alignment. The default setting for this is "Auto."
		fa_fahang,				//fahang	Font alignment: Hanging.
		fa_facenter,			//facenter	Font alignment: Center.
		fa_faroman,				//faroman	Font alignment: Roman (default).
		fa_favar,				//favar	Font alignment: Upholding variable.
		fa_fafixed,				//fafixed	Font alignment: Upholding fixed.
	} FontAlign;
	
	FontAlign m_eFontAlign;

	int m_nIndFirstLine;		//fiN	First-line indent in twips (default is 0).
	int m_nIndLeft;				//liN	Left indent in twips (default is 0).
	int m_nIndRight;			//riN	Right indent in twips (default is 0).
	int m_nIndStart;			//linN	Left indent for left-to-right paragraphs; right indent for right-to-left paragraphs (default is 0). \linN defines space before the paragraph.
	int m_nIndEnd;				//rinN	Right indent for left-to-right paragraphs; left indent for right-to-left paragraphs (default is 0). \rinN defines space after the paragraph.
	int m_bIndRightAuto;		//adjustright	Automatically adjust right indent when document grid is defined.
	int m_bIndMirror;			//indmirror...

	int m_nSpaceBefore;			//sbN	Space before in twips (default is 0).
	int m_nSpaceAfter;			//saN	Space after in twips (default is 0).
	int m_nSpaceBeforeAuto;		//sbautoN	Auto spacing before:
	int m_nSpaceAfterAuto;		//saautoN	Auto spacing after:
	int m_nSpaceBeforeLine;		//lisbN	Space before in hundredths of a character unit. Overrides \sbN, although they should both be emitted with equivalent values.
	int m_nSpaceAfterLine;		//lisaN	Space after in hundredths of a character unit. Overrides \saN, although they should both be emitted with equivalent values.
	int m_nSpaceBetween;		//slN	Space between lines. If this control word is missing or if \sl0 is used, the line spacing is automatically determined by the tallest character in the line. If N is a positive value, this size is used only if it is taller than the tallest character (otherwise, the tallest character is used); if N is a negative value, the absolute value of N is used, even if it is shorter than the tallest character.
	int m_nSpaceMultiLine;		//slmultN
	int m_bContextualSpacing;	//contextualspace

	int m_bRtl;					//rtlpar	Text in this paragraph will display with right-to-left precedence.
	int m_bNoWordWrap;			//nowwrap
	int m_bSnapToGrid;			//nosnaplinegrid	Disable snap line to grid.

	enum _TextBoxWrap
	{
		tbw_none,
		tbw_txbxtwno,			//txbxtwno
		tbw_txbxtwalways,		//txbxtwalways
		tbw_txbxtwfirstlast,	//txbxtwfirstlast
		tbw_txbxtwfirst,		//txbxtwfirst
		tbw_txbxtwlast,			//txbxtwlast
	}			m_eTextBoxWrap;
	int			m_nListId;					//lsN	Should exactly match the \lsN for one of the list overrides in the List Override table.
	int			m_nListLevel;				//ilvlN	The 0-based level of the list to which the paragraph belongs. For all simple lists, N should always be 0. For multilevel lists, it can be 0 through 8. The value 9 is never used. The values 10 through 12 have the special meanings for documents generated by Word 6: 10 = ilvlBullet (a bulleted paragraph in Word 6), 11 = ilvlList (a numbered paragraph in Word 6), 12 = ilvlContinue (a paragraph that was not itself numbered, but took its indenting scheme from its numbering properties and did not “break” numbering (that in Word 6 required otherwise contiguous paragraphs).

	RtfShadingPar m_oShading;

	RtfBorder m_oBorderTop;
	RtfBorder m_oBorderLeft;
	RtfBorder m_oBorderBottom;
	RtfBorder m_oBorderRight;
	RtfBorder m_oBorderBox;
	RtfBorder m_oBorderBar;

	RtfFrame	m_oFrame;
	int			m_bOverlap;				//1\absnoovrlpN	Allow overlap with other frames or objects with similar wrapping:
	enum _TextFollow
	{
		tf_none,
		tf_frmtxlrtb,		//frmtxlrtb	Frame box flows from left to right and top to bottom (default).
		tf_frmtxtbrl,		//frmtxtbrl	Frame box flows right to left and top to bottom.
		tf_frmtxbtlr,		//frmtxbtlr	Frame box flows left to right and bottom to top.
		tf_frmtxlrtbv,		//frmtxlrtbv	Frame box flows left to right and top to bottom, vertical.
		tf_frmtxtbrlv		//frmtxtbrlv	Frame box flows top to bottom and right to left, vertical.
	}			m_eTextFollow;
	RtfTabs		m_oTabs;

//Table Style Specific
	int m_nTableStyle;			// ytsN	Designates the table style handle that was applied to the row/cell.
	int m_bStyleFirstRow;		// tscfirstrow	This cell is in the first row.
	int m_bStyleLastRow;		// tsclastrow	This cell is in the last row.
	int m_bStyleFirstCollumn;	// tscfirstcol	This cell is in the first column.
	int m_bStyleLastCollumn;	// tsclastcol	This cell is in the last column.
	int m_bStyleOddRowBand;		// tscbandhorzodd	This cell is in the odd row band.
	int m_bStyleEvenRowBand;	// tscbandhorzeven	This cell is in the even row band.
	int m_bStyleOddColBand;		// tscbandvertodd	This cell is in the odd column band.
	int m_bStyleEvenColBand;	// tscbandverteven	This cell is in the even column band.
	int m_bStyleNWCell;			// tscnwcell	This is the NW (north west) cell in the table (upper left).
	int m_bStyleNECell;			// tscnecell	NE cell.
	int m_bStyleSWCell;			// tscswcell	SW cell.
	int m_bStyleSECell;			// tscsecell	SE cell.
	
	int m_nPrAuth;
	int m_nPrDate;
	
	RtfCharProperty		m_oCharProperty;

	//--------------------------------------------------------------------------------------------------------------

	RtfParagraphProperty();
	bool IsValid();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

	void Merge( RtfParagraphProperty& oParPr );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

//class RtfParagraphPropertyWithTable: public RtfParagraphProperty
//{
//public: RtfRowProperty m_oRowProperty;
//public: RtfCellProperty m_oCellProperty;
//
//public: RtfParagraphPropertyWithTable()
//		{
//			SetDefault();
//		}
//public: void SetDefaultRtfPar()
//		{
//			RtfParagraphProperty::SetDefaultRtf();
//		}
//public: void SetDefaultRtfTable()
//		{
//			m_oRowProperty.SetDefaultRtf();
//		}
//public: void SetDefault()
//		{
//			RtfParagraphProperty::SetDefault();
//			m_oRowProperty.SetDefault();
//		}
//public: std::wstring RenderToRtf(RenderParameter oRenderParameter)
//		{
//			std::wstring sResult;
//			sResult += m_oRowProperty.RenderToRtf(  oRenderParameter  );
//			sResult += L" {" + RtfParagraphProperty::RenderToRtf(  oRenderParameter  ) + L" }";
//			return sResult;
//		}
//};

class RtfTime: public IRenderableProperty
{
public: 
	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_nHour;
	int m_nMin;
	int m_nSecond;

	RtfTime();
	bool IsValid();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfInformation: public IRenderableProperty
{
public:
    std::wstring m_sTitle;
    std::wstring m_sSubject;
    std::wstring m_sAuthor;
    std::wstring m_sManager;
    std::wstring m_sCompany;
    std::wstring m_sOperator;
    std::wstring m_sCategory;
    std::wstring m_sKeywords;
    std::wstring m_sComment;
    std::wstring m_sDocCom;
    std::wstring m_sLinkBase;
	
	std::wstring m_sApplication;

	RtfTime m_oCreateTime;
	RtfTime m_oRevTime;
	RtfTime m_oPrintTime;
	RtfTime m_oBackupTime;

	int m_nVersion;
	int m_nInternalVersion;
	int m_nEndingTime;

	int m_nNumberOfPages;
	int m_nNumberOfWords;
	int m_nNumberOfCharactersWithSpace;
	int m_nNumberOfCharactersWithoutSpace;
	int m_nInternalId;

	RtfInformation();
	bool IsValid();

	void SetDefaultRtf();
	void SetDefaultOOX();
	void SetDefault();

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfCharStyle: public RtfStyle
{
public: 
	RtfCharProperty m_oCharProp;

	RtfCharStyle();

	int GetType();
	void Merge( RtfStylePtr oStyle );

    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

class RtfParagraphStyle: public RtfCharStyle
{
public: 
	RtfParagraphProperty m_oParProp;

	RtfParagraphStyle();

	int GetType();
	void Merge( RtfStylePtr oStyle );
	
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

//---------------------------------------------------------------------------------------
class RtfTableStyle;
typedef boost::shared_ptr<RtfTableStyle> RtfTableStylePtr;

class RtfTableStyle: public RtfParagraphStyle
{
public: 
	RtfTableProperty	m_oTableProp;
	RtfRowProperty		m_oRowProp;
	RtfCellProperty		m_oCellProp;


	RtfTableStylePtr m_oFirstRow;
	RtfTableStylePtr m_oLastRow;
	RtfTableStylePtr m_oFirstCol;
	RtfTableStylePtr m_oLastCol;
	RtfTableStylePtr m_oBandHorEven;
	RtfTableStylePtr m_oBandVerEven;
	RtfTableStylePtr m_oBandHorOdd;
	RtfTableStylePtr m_oBandVerOdd;
	RtfTableStylePtr m_oNWCell;
	RtfTableStylePtr m_oNECell;
	RtfTableStylePtr m_oSWCell;
	RtfTableStylePtr m_oSECell;

	RtfTableStyle();

	void SetDefault();
	int GetType();
	void Merge( RtfStylePtr oStyle );
	
    std::wstring RenderToRtf(RenderParameter oRenderParameter);
    std::wstring RenderToOOX(RenderParameter oRenderParameter);
};

//typedef boost::shared_ptr<RtfTableStyleProperty>	RtfTableStylePropertyPtr;
typedef boost::shared_ptr<RtfCharStyle>				RtfCharStylePtr;
typedef boost::shared_ptr<RtfParagraphStyle>		RtfParagraphStylePtr;
typedef boost::shared_ptr<RtfInformation>			RtfInformationPtr;
typedef boost::shared_ptr<RtfTime>					RtfTimePtr;
typedef boost::shared_ptr<RtfRowProperty>			RtfRowPropertyPtr;
typedef boost::shared_ptr<RtfCellProperty>			RtfCellPropertyPtr;
typedef boost::shared_ptr<RtfFrame>					RtfFramePtr;
typedef boost::shared_ptr<RtfTableProperty>			RtfTablePropertyPtr;
typedef boost::shared_ptr<RtfTab>					RtfTabPtr;

typedef boost::shared_ptr<RtfListOverrideProperty>	RtfListOverridePropertyPtr;
typedef boost::shared_ptr<RtfListProperty>			RtfListPropertyPtr;
typedef boost::shared_ptr<RtfListLevelProperty>		RtfListLevelPropertyPtr;
typedef boost::shared_ptr<RtfBorder>				RtfBorderPtr;
typedef boost::shared_ptr<RtfShadingTableStyle>		RtfShadingTableStylePtr;
typedef boost::shared_ptr<RtfShadingRow>			RtfShadingRowPtr;
typedef boost::shared_ptr<RtfShadingCell>			RtfShadingCellPtr;
typedef boost::shared_ptr<RtfShadingChar>			RtfShadingCharPtr;
typedef boost::shared_ptr<RtfShadingPar>			RtfShadingParPtr;
typedef boost::shared_ptr<RtfShading>				RtfShadingPtr;
typedef boost::shared_ptr<RtfColor>					RtfColorPtr;
typedef boost::shared_ptr<RtfFont>					RtfFontPtr;
