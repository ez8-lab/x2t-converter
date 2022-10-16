#include "x2tTester.h"
#include "../../../X2tConverter/src/run.h"

class FormatsList;
class Cx2tTester;
class CConverter;

FormatsList::FormatsList()
{
}

std::vector<int> FormatsList::GetDocuments() const
{
	return m_documents;
}
std::vector<int> FormatsList::GetPresentations() const
{
	return m_presentations;
}
std::vector<int> FormatsList::GetSpreadsheets() const
{
	return m_spreadsheets;
}
std::vector<int> FormatsList::GetCrossplatform() const
{
	return m_crossplatform;
}
std::vector<int>FormatsList:: GetImages() const
{
	return m_images;
}

bool FormatsList::isDocument(int format) const
{
	return std::find(m_documents.begin(), m_documents.end(), format) != m_documents.end();
}
bool FormatsList::isPresentation(int format) const
{
	return std::find(m_presentations.begin(), m_presentations.end(), format) != m_presentations.end();
}
bool FormatsList::isSpreadsheet(int format) const
{
	return std::find(m_spreadsheets.begin(), m_spreadsheets.end(), format) != m_spreadsheets.end();
}
bool FormatsList::isCrossplatform(int format) const
{
	return std::find(m_crossplatform.begin(), m_crossplatform.end(), format) != m_crossplatform.end();
}
bool FormatsList::isImage(int format) const
{
	return std::find(m_images.begin(), m_images.end(), format) != m_images.end();
}

void FormatsList::SetDefault()
{
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCX);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOC);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_ODT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_RTF);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_TXT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_HTML);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_MHT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_EPUB);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_FB2);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_MOBI);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCM);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOTX);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOTM);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_ODT_FLAT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOC_FLAT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCX_FLAT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_HTML_IN_CONTAINER);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCX_PACKAGE);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_OTT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_OFORM);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCXF);

	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPTX);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPT);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_ODP);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPSX);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPTM);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPSM);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_POTX);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_POTM);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_ODP_FLAT);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_OTP);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPTX_PACKAGE);

	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSX);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLS);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_ODS);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_CSV);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSM);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLTX);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLTM);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSB);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_ODS_FLAT);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_OTS);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSX_FLAT);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSX_PACKAGE);

	m_crossplatform.push_back(AVS_OFFICESTUDIO_FILE_CROSSPLATFORM_PDF);

	m_images.push_back(AVS_OFFICESTUDIO_FILE_IMAGE_JPG);
	m_images.push_back(AVS_OFFICESTUDIO_FILE_IMAGE_PNG);
}

void FormatsList::SetOutput()
{
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCX);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOC);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_ODT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_RTF);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_TXT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_HTML);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_MHT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_EPUB);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_FB2);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_MOBI);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCM);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOTX);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOTM);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_ODT_FLAT);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOC_FLAT);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCX_FLAT);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_HTML_IN_CONTAINER);
//	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCX_PACKAGE);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_OTT);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_OFORM);
	m_documents.push_back(AVS_OFFICESTUDIO_FILE_DOCUMENT_DOCXF);

	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPTX);
//	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPT);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_ODP);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPSX);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPTM);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPSM);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_POTX);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_POTM);
//	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_ODP_FLAT);
	m_presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_OTP);
//	presentations.push_back(AVS_OFFICESTUDIO_FILE_PRESENTATION_PPTX_PACKAGE);

	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSX);
//	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLS);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_ODS);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_CSV);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSM);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLTX);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLTM);
//	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSB);
//	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_ODS_FLAT);
	m_spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_OTS);
//	spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSX_FLAT);
//	spreadsheets.push_back(AVS_OFFICESTUDIO_FILE_SPREADSHEET_XLSX_PACKAGE);

	m_crossplatform.push_back(AVS_OFFICESTUDIO_FILE_CROSSPLATFORM_PDF);

	m_images.push_back(AVS_OFFICESTUDIO_FILE_IMAGE_JPG);
	m_images.push_back(AVS_OFFICESTUDIO_FILE_IMAGE_PNG);
}
std::vector<int> FormatsList::allFormats() const
{
	std::vector<int> all_formats;

	for(auto val : m_documents)
		all_formats.push_back(val);

	for(auto val : m_presentations)
		all_formats.push_back(val);

	for(auto val : m_spreadsheets)
		all_formats.push_back(val);

	for(auto val : m_images)
		all_formats.push_back(val);

	for(auto val : m_crossplatform)
		all_formats.push_back(val);

	return all_formats;
}


Cx2tTester::Cx2tTester(const std::wstring& configPath)
{
	m_bIsUseSystemFonts = true;
	m_bIsErrorsOnly = false;
	m_inputFormatsList.SetDefault();
	m_outputFormatsList.SetOutput();
	setConfig(configPath);

	if(NSFile::CFileBinary::Exists(m_reportFile))
		NSFile::CFileBinary::Remove(m_reportFile);

	if(!m_reportStream.CreateFileW(m_reportFile))
	{
		std::wcerr << "Report file is not open!" << std::endl;
		exit(-1);
	}

	m_coresCS.InitializeCriticalSection();
	m_reportCS.InitializeCriticalSection();
	m_outputCS.InitializeCriticalSection();

	m_currentProc = 0;
	setReportHeader();
}
Cx2tTester::~Cx2tTester()
{
	m_coresCS.DeleteCriticalSection();
	m_reportCS.DeleteCriticalSection();
	m_outputCS.DeleteCriticalSection();

	m_reportStream.CloseFile();
}

void Cx2tTester::setConfig(const std::wstring& configPath)
{
	bool default_input_formats = true;
	bool default_output_formats = true;

	XmlUtils::CXmlNode root;
	XmlUtils::CXmlNodes nodes;
	if(root.FromXmlFile(configPath) && root.GetChilds(nodes))
	{
		for(int i = 0; i < nodes.GetCount(); i++)
		{
			XmlUtils::CXmlNode node;
			nodes.GetAt(i, node);
			std::wstring name = node.GetName();

			// key-value
			if(name == L"reportPath") m_reportFile = node.GetText();
			else if(name == L"inputDirectory") m_inputDirectory = node.GetText();
			else if(name == L"outputDirectory") m_outputDirectory = node.GetText();
			else if(name == L"x2tPath") m_x2tPath = node.GetText();
			else if(name == L"cores") m_maxProc = std::stoi(node.GetText());
			else if(name == L"errorsOnly") m_bIsErrorsOnly = std::stoi(node.GetText());
			else if(name == L"input")
			{
				default_input_formats = false;
				std::wstring extensions = node.GetText();
				extensions += L' ';
				m_inputFormats = parseExtensionsString(extensions, m_inputFormatsList);
			}
			else if(name == L"output")
			{
				default_output_formats = false;
				std::wstring extensions = node.GetText();
				extensions += L' ';
				m_outputFormats = parseExtensionsString(extensions, m_outputFormatsList);
			}
			else if (name == L"fonts")
			{
				m_bIsUseSystemFonts = (1 == node.ReadValueInt(L"system", 1)) ? true : false;
				XmlUtils::CXmlNodes oNodeFontDirs = node.ReadNodesNoNS(L"directory");
				for (int nIndex = 0, nCount = oNodeFontDirs.GetCount(); nIndex < nCount; ++nIndex)
				{
					XmlUtils::CXmlNode oNodeDir;
					oNodeFontDirs.GetAt(nIndex, oNodeDir);
					m_arAdditionalFontsDirs.push_back(oNodeDir.GetText());
				}
			}
		}
	}
	// else err

	if(default_input_formats)
		m_inputFormats = m_inputFormatsList.allFormats();

	if(default_output_formats)
		m_outputFormats = m_outputFormatsList.allFormats();


}
void Cx2tTester::Start()
{
	// check fonts
	std::wstring fonts_directory = NSFile::GetProcessDirectory() + L"/fonts";
	CApplicationFontsWorker fonts_worker;
	fonts_worker.m_sDirectory = fonts_directory;
	if (!NSDirectory::Exists(fonts_worker.m_sDirectory))
		NSDirectory::CreateDirectory(fonts_worker.m_sDirectory);

	fonts_worker.m_bIsUseSystemFonts = m_bIsUseSystemFonts;

	for (std::vector<std::wstring>::iterator i = m_arAdditionalFontsDirs.begin(); i != m_arAdditionalFontsDirs.end(); i++)
		fonts_worker.m_arAdditionalFolders.push_back(*i);

	fonts_worker.m_bIsNeedThumbnails = false;
	NSFonts::IApplicationFonts* pFonts = fonts_worker.Check();
	RELEASEINTERFACE(pFonts);

	// setup & clear output folder
	if(NSDirectory::Exists(m_outputDirectory))
		NSDirectory::DeleteDirectory(m_outputDirectory);

	NSDirectory::CreateDirectory(m_outputDirectory);

	std::vector<std::wstring> files = NSDirectory::GetFiles(m_inputDirectory, false);

	if(files.size() < m_maxProc)
		m_maxProc = files.size();

	for(std::wstring& input_file : files)
	{
		std::wstring input_ext = NSFile::GetFileExtention(input_file);
		int input_format = COfficeFileFormatChecker::GetFormatByExtension(L'.' + input_ext);

		// if no format in input formats - skip
		if(std::find(m_inputFormats.begin(), m_inputFormats.end(), input_format) == m_inputFormats.end())
			continue;

		// setup folder for output files
		std::wstring output_files_directory = m_outputDirectory + L'/' + NSFile::GetFileName(input_file);

		if(NSDirectory::Exists(output_files_directory))
			NSDirectory::DeleteDirectory(output_files_directory);

		NSDirectory::CreateDirectory(output_files_directory);

		// setup output_formats for file
		std::vector<int> output_file_formats;

		// documents -> documents, images, crossplatform
		// presentations -> presentations, images, crossplatfrom
		// spreadsheets -> spreadsheets, images, crossplatform
		for(auto format : m_outputFormats)
		{
			if((m_outputFormatsList.isDocument(format) && m_inputFormatsList.isDocument(input_format))
			|| (m_outputFormatsList.isSpreadsheet(format) && m_inputFormatsList.isSpreadsheet(input_format))
			|| (m_outputFormatsList.isPresentation(format) && m_inputFormatsList.isPresentation(input_format))
			|| m_outputFormatsList.isImage(format)
			|| m_outputFormatsList.isCrossplatform(format))
			{
				output_file_formats.push_back(format);
			}
		}


		// waiting...
		do
			NSThreads::Sleep(150);
		while(isAllBusy());

		// setup & start new coverter
		m_currentProc++;
		CConverter *converter = new CConverter(this);
		converter->SetInputFile(input_file);
		converter->SetInputFormat(input_format);
		converter->SetOutputFilesDirectory(output_files_directory);
		converter->SetOutputFormats(output_file_formats);
		converter->SetFontsDirectory(fonts_directory);
		converter->SetX2tPath(m_x2tPath);
		converter->SetOnlyErrors(m_bIsErrorsOnly);

		converter->DestroyOnFinish();
		converter->Start(0);
	}

	// waiting all procs end
	while(!isAllFree())
		NSThreads::Sleep(150);
}

void Cx2tTester::writeReport(const Report& report)
{
	CTemporaryCS CS(&m_reportCS);

	m_reportStream.WriteStringUTF8(report.inputFile + L"\t", true);
	m_reportStream.WriteStringUTF8(report.outputFile + L"\t", true);
	m_reportStream.WriteStringUTF8(report.inputExt + L"\t", true);
	m_reportStream.WriteStringUTF8(report.outputExt + L"\t", true);
	m_reportStream.WriteStringUTF8(std::to_wstring(report.exitCode) + L"\n", true);
}
void Cx2tTester::writeReports(const std::vector<Report>& reports)
{
	CTemporaryCS CS(&m_reportCS);
	for(auto& report : reports)
	{
		m_reportStream.WriteStringUTF8(report.inputFile + L"\t", true);
		m_reportStream.WriteStringUTF8(report.outputFile + L"\t", true);
		m_reportStream.WriteStringUTF8(report.inputExt + L"\t", true);
		m_reportStream.WriteStringUTF8(report.outputExt + L"\t", true);
		m_reportStream.WriteStringUTF8(std::to_wstring(report.exitCode) + L"\n", true);
	}
}

bool Cx2tTester::isAllBusy()
{
	CTemporaryCS CS(&m_coresCS);
	return m_currentProc == m_maxProc;
}
bool Cx2tTester::isAllFree()
{
	CTemporaryCS CS(&m_coresCS);
	return m_currentProc == 0;
}

void Cx2tTester::setReportHeader()
{
	m_reportStream.WriteStringUTF8(L"Input file\t", false);
	m_reportStream.WriteStringUTF8(L"Output file\t", true);
	m_reportStream.WriteStringUTF8(L"Input extension\t", true);
	m_reportStream.WriteStringUTF8(L"Output extension\t", true);
	m_reportStream.WriteStringUTF8(L"Exit code\n", true);
}

std::vector<int> Cx2tTester::parseExtensionsString(std::wstring extensions, const FormatsList& fl)
{
	std::vector<int> formats;
	int pos = 0;
	while ((pos = extensions.find(' ')) != std::wstring::npos)
	{
		std::wstring ext = extensions.substr(0, pos);

		if(ext == L"documents")
			formats = fl.GetDocuments();

		else if(ext == L"presentations")
			formats = fl.GetPresentations();

		else if(ext == L"spreadsheets")
			formats = fl.GetSpreadsheets();

		else
		{
			int format =  COfficeFileFormatChecker::GetFormatByExtension(L'.' + ext);
			formats.push_back(format);
		}
		extensions.erase(0, pos + 1);
	}
	return formats;
}


CConverter::CConverter(Cx2tTester* internal) : m_internal(internal), m_bIsErrorsOnly(false)
{
}
CConverter::~CConverter()
{
	Stop();
}

void CConverter::SetInputFile(const std::wstring& inputFile)
{
	m_inputFile = inputFile;
}
void CConverter::SetInputFormat(int inputFormat)
{
	m_inputFormat = inputFormat;
}

void CConverter::SetOutputFilesDirectory(const std::wstring& outputFilesDirectory)
{
	m_outputFilesDirectory = outputFilesDirectory;
}
void CConverter::SetOutputFormats(const std::vector<int> outputFormats)
{
	m_outputFormats = outputFormats;
}

void CConverter::SetFontsDirectory(const std::wstring& fontsDirectory)
{
	m_fontsDirectory = fontsDirectory;
}
void CConverter::SetX2tPath(const std::wstring& x2tPath)
{
	m_x2tPath = x2tPath;
}
void CConverter::SetOnlyErrors(bool bIsErrorsOnly)
{
	m_bIsErrorsOnly = bIsErrorsOnly;
}

DWORD CConverter::ThreadProc()
{
	std::wstring xml_params_file = m_outputFilesDirectory + L"/params.xml";
	std::vector<Cx2tTester::Report> reports;

	// input_format in many output exts
	for(int& output_format : m_outputFormats)
	{
		if(m_inputFormat == output_format)
			continue;

		std::wstring output_file = m_outputFilesDirectory
				+ L"/" + NSFile::GetFileName(m_inputFile) + checker.GetExtensionByType(output_format);

		// creating temporary xml file with params
		NSStringUtils::CStringBuilder builder;
		builder.WriteString(L"<?xml version=\"1.0\" encoding=\"utf-8\"?>");

		builder.WriteString(L"<Root>");

		builder.WriteString(L"<m_sFileFrom>");
		builder.WriteEncodeXmlString(m_inputFile);
		builder.WriteString(L"</m_sFileFrom>");

		builder.WriteString(L"<m_sFileTo>");
		builder.WriteEncodeXmlString(output_file);
		builder.WriteString(L"</m_sFileTo>");

		builder.WriteString(L"<m_nFormatFrom>");
		builder.WriteString(std::to_wstring(m_inputFormat));
		builder.WriteString(L"</m_nFormatFrom>");

		builder.WriteString(L"<m_nFormatTo>");
		builder.WriteString(std::to_wstring(output_format));
		builder.WriteString(L"</m_nFormatTo>");

		builder.WriteString(L"<m_sThemeDir>./</m_sThemeDir>");
		builder.WriteString(L"<m_bDontSaveAdditional>true</m_bDontSaveAdditional>");

		builder.WriteString(L"<m_sAllFontsPath>");
		builder.WriteEncodeXmlString(m_fontsDirectory + L"/AllFonts.js");
		builder.WriteString(L"</m_sAllFontsPath>");

		builder.WriteString(L"<m_sFontDir>");
		builder.WriteEncodeXmlString(m_fontsDirectory);
		builder.WriteString(L"</m_sFontDir>");

		builder.WriteString(L"<m_oThumbnail><format>4</format><aspect>2</aspect><first>false</first><width>1000</width><height>1000</height></m_oThumbnail>");
		builder.WriteString(L"<m_sJsonParams>{&quot;spreadsheetLayout&quot;:{&quot;gridLines&quot;:true,&quot;headings&quot;:true,&quot;fitToHeight&quot;:1,&quot;fitToWidth&quot;:1,&quot;orientation&quot;:&quot;landscape&quot;}}</m_sJsonParams>");

		builder.WriteString(L"</Root>");

		std::wstring xml_params = builder.GetData();

		NSFile::CFileBinary::SaveToFile(xml_params_file, xml_params, true);

		int exit_code = NSX2T::Convert(NSFile::GetDirectoryName(m_x2tPath), xml_params_file);

		// writing report
		if(!m_bIsErrorsOnly || exit_code)
		{
			Cx2tTester::Report report;
			report.inputFile = m_inputFile;
			report.outputFile = output_file;
			report.inputExt = checker.GetExtensionByType(m_inputFormat);
			report.outputExt = checker.GetExtensionByType(output_format);
			report.exitCode = exit_code;
			reports.push_back(report);
		}

		std::string input_file_UTF8 = U_TO_UTF8(m_inputFile);
		std::string output_file_UTF8 = U_TO_UTF8(output_file);

		CTemporaryCS CS(&m_internal->m_outputCS);
		std::cout << input_file_UTF8 << " to " << output_file_UTF8 << " ";

		if(!exit_code)
			std::cout << "OK";
		else
			std::cout << "BAD";

		std::cout << std::endl;

	}
	NSFile::CFileBinary::Remove(xml_params_file);
	m_internal->writeReports(reports);
	m_internal->m_currentProc--;
	return 0;
}


