
#include "fxqa_xmlparser.h"
#include "fxqa_argparser.h"
#include "fxqa_testmanager.h"
#include "fxqa_testlog.h"
#include "demo.h"
#include "fxqa_charset.h"

#if defined(_FXQA_PLATFORM_QTIOS_)
#include <QApplication>
#include <QStandardPaths>
#endif

#if defined(_FXQA_PLATFORM_WINDOWS_WIN32_) && defined(_FXQA_TOOL_VLD_)
    #include "vld.h"
#endif

#ifdef _FXQA_PLATFORM_WINDOWS_RT_
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Storage;
#endif

std::string FXQAGetFilter(char** argv) {
	const char* filter = "*.*";
#if defined(_FXQA_PLATFORM_QTIOS_) || defined(_FXQA_PLATFORM_QTANDROID_) || defined(_FXQA_PLATFORM_WINDOWS_RT_)
    FILE* fp = NULL;
#   if	defined(_FXQA_PLATFORM_QTANDROID_)
    fp = fopen("/sdcard/qatest/qa_test_filter", "r");
#	elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	ApplicationData^  appData = Windows::Storage::ApplicationData::Current;
	Windows::Storage::StorageFolder^ appLocalFolder = appData->LocalFolder;
	String^ appLocalPath = appLocalFolder->Path;
	appLocalPath += "/qa_test_filter";
	_wfopen_s(&fp, appLocalPath->Data(), L"r");
#   else
    QStringList ios_doc_path_qsl = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    QString ios_doc_path_qs = ios_doc_path_qsl.join("");
    std::string ios_doc_path = ios_doc_path_qs.toUtf8().constData();
    fp = fopen((ios_doc_path + "/qatest/qa_test_filter").c_str(), "r");
#   endif
    if (fp != NULL) {
        char fbuf[256];
        int len = fread(fbuf, 1, 256, fp);
        fbuf[len] = '\0';
        filter = fbuf;
        fclose(fp);
    } else {
        filter = "*.*";
    }
#else
    if (argv[1] != NULL) {
        filter = argv[1];
    }
#endif

	return filter;
}

int main(int argc, char** argv)
{
#if defined(_FXQA_PLATFORM_QTANDROID_)
	std::string xml_path = "/sdcard/qatest/autotest.xml";
#elif defined(_FXQA_PLATFORM_QTIOS_)
	QStringList ios_doc_path_qsl = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
	QString ios_doc_path_qs = ios_doc_path_qsl.join("");
	std::string ios_doc_path = ios_doc_path_qs.toUtf8().constData();
	std::string xml_path = ios_doc_path + "/qatest/autotest.xml";
#elif defined(_FXQA_PLATFORM_WINDOWS_RT_)
	ApplicationData^  appData = Windows::Storage::ApplicationData::Current;
	Windows::Storage::StorageFolder^ appLocalFolder = appData->LocalFolder;
	String^ appLocalPath = appLocalFolder->Path;
	appLocalPath += "/autotest.xml";
	std::string xml_path = ws2s(appLocalPath->Data());
#else
	std::string xml_path = "autotest.xml";
#endif

#if defined(_FXQA_MICROSERVICE_)
		fxqa_test::CARGParser argParser(argc, argv);
		QA_TEST_filter(QATEST_COMPARE::CFXQACompare) = "*.*";
		fxqa_test::CXMLParser* xmlParseArg = argParser.Convert2XMLParser();
		RUN_TEST(QATEST_COMPARE::CFXQACompare, xmlParseArg);
		return 0;
#endif // _FXQA_MICROSERVICE_

		fxqa_test::CXMLParser* xmlParse = new fxqa_test::CXMLParser(xml_path);
		if (xmlParse->IsLoadXMLConfig() == false) {
			QA_LOG()  << "XML LOAD FAILED.\n";
			return -1;
		}
	
		QA_LOG_path = xmlParse->GetPath_Report();
		QA_TEST_filter(QATEST_COMPARE::CFXQACompare) = FXQAGetFilter(argv);
		RUN_TEST(QATEST_COMPARE::CFXQACompare, xmlParse);

		FXQA_printf("Test Normal End.");
		delete xmlParse;
		
	return 0;
}

