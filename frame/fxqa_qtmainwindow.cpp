#include "fxqa_qtmainwindow.h"
#include "ui_mainwindow.h"
#include "fxqa_include.h"
#include "fxqa_interface.h"
#include "render_new.h"
#include "fxqa_charset.h"

#include <dlfcn.h>
#ifdef _FXQA_PLATFORM_QTANDROID_
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif
#include <QDebug>
#include <QFile>


typedef IPDF_Compare*  (*DLLFUN)(void);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
#ifdef _FXQA_PLATFORM_ANDROID_
    QAndroidJniObject mediaDir = QAndroidJniObject::callStaticObjectMethod("android/os/Environment", "getDataDirectory", "()Ljava/io/File;");
   QString sdcardPath = mediaDir.toString() + "/lib/libtestLib.so";
     //QString sdcardPath = "ffffffffffff";
    //qDebug() << sdcardPath;

    QByteArray ba = sdcardPath.toLatin1();
    const char* c_str2 = ba.data();
    c_str2 = "/data/data/qalib/libtestLib.so";

/*
    const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(sdcardPath.utf16());
        std::wstring filePath = s2ws(std::string(c_str2));
     std::string tem =   ws2s(filePath);
    //qDebug(L"wrg:%s", filePath.c_str());
    qDebug("sdfgwrg:%s", tem.c_str());
    */

    QFile Fout(c_str2);
    if(!Fout.exists()) {
        qDebug() << "No exist.";
    } else {
        qDebug() << "exist.";
    }

    void * libHND = dlopen(c_str2, RTLD_LAZY | RTLD_GLOBAL);
            if (!libHND) {
            qDebug(dlerror());
           // exit(1);
        }

     void* funcAddress = dlsym((void*)libHND, "PDF_RenderCompare_Create_NewAPI");
     if (!funcAddress) {
       qDebug(dlerror());
     } else {
         qDebug() << "Load qalib Success!!!";
     }

     DLLFUN  Dll_clFun_New = (DLLFUN)funcAddress;
     qDebug() << "funcAddress";
     CPDF_RenderCompare_NewAPI* m_newAPI = (CPDF_RenderCompare_NewAPI*)Dll_clFun_New();
    if (m_newAPI) {
        qDebug() << "Get RenderCompare Success!!!";
    } else {
        qDebug() << "Get RenderCompare ERROR!!!";
    }

     CPDF_Parser* pParser = m_newAPI->CreateParser();
     if (pParser) {
        qDebug() << "CreateParser Success!!!";
     } else {
        qDebug() << "CreateParser ERROR!!!";
     }

     QString testfilePathQ = mediaDir.toString() + "/qalib/FoxitForm.pdf";

     QByteArray ba0 = testfilePathQ.toLatin1();
     const char* c_str20 = ba0.data();

     //const wchar_t * encodedName = reinterpret_cast<const wchar_t *>(testfilePathQ.utf8());
    std::wstring filePath = s2ws(std::string(c_str20));

     qDebug() << "TestFile: " << testfilePathQ;
     qDebug() << "TestFile_0: " << c_str20;

     FX_INT32 retParseOld = m_newAPI->ParseDocument(pParser, c_str20);
     qDebug() << "ParseDocument Return: " << retParseOld;
     retParseOld = m_newAPI->ParseDocument(pParser, L"/storage/emulated/0/qalib/FoxitForm.pdf");
     qDebug() << "ParseDocument Return2: " << retParseOld;

     retParseOld = m_newAPI->ParseDocument(pParser, (FX_LPCWSTR)filePath.c_str());
     qDebug() << "ParseDocument Return3: " << retParseOld;
  //  CPDF_Document* pDocOld = NULL;
 //    ret = m_oldAPI->GetPDFDocument(pOldParser, pDocOld);

#endif

}

int qa_main(int argc, char** argv);
MainWindow::MainWindow(int argc, char **argv, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
     ui->setupUi(this);
     qa_main(argc, argv);
    qDebug() << "asdfasf";
}

MainWindow::~MainWindow()
{
    delete ui;
}
