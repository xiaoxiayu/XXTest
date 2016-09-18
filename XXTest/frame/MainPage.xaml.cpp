//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"



#include "fxqa_interface.h"
#include "render_new.h"
using namespace App4;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
//using namespace newapi;

using namespace concurrency;
using namespace Windows::Storage;
using namespace Windows::Storage::Streams;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Storage::Pickers;
using namespace Windows::ApplicationModel;


// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

typedef IPDF_Compare*  (*DLLFUN)(void);

MainPage::MainPage()
{
	InitializeComponent();

	//FileOpenPicker^ openPicker = ref new FileOpenPicker();
	//openPicker->ViewMode = PickerViewMode::Thumbnail;
	//openPicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
	//openPicker->FileTypeFilter->Append(".pdf");
	//openPicker->FileTypeFilter->Append(".jpeg");
	//openPicker->FileTypeFilter->Append(".png");

	//create_task(openPicker->PickSingleFileAsync()).then([this](StorageFile ^file)
	//{
	//	if (file)
	//	{
	//		//OutputTextBlock->Text = "Picked photo: " + file->Name;
	//		create_task(file->CopyAsync((Windows::Storage::ApplicationData::Current)->TemporaryFolder, file->Name, Windows::Storage::NameCollisionOption::ReplaceExisting)).then([this](StorageFile ^file2)
	//		{
	//			if (file2)
	//			{
	//				//OutputTextBlock->Text = "Copied File to: " + file2->Path;
	//				auto ls = file2->Path->Data();
	//				FILE *pFile;
	//				HRESULT hr = _wfopen_s(&pFile, ls, L"rb");
	//				//fread_s code was here
	//				fclose(pFile);
	//			}
	//		});
	//	}
	//	else
	//	{
	//		//OutputTextBlock->Text = "Operation cancelled.";
	//	}
	//});
	

	FILE *pFile;
	HRESULT hr = _wfopen_s(&pFile, L"C:\\Users\\xiaoxia\\AppData\\Local\\Packages\\069c94df-2c92-4d83-97d6-2dad9f36099c_td1h08zsn3e00\\TempState\\1.pdf",  L"rb");


	Package^ package = Package::Current;
	String^ AppXPath = package->InstalledLocation->Path;
	AppXPath += "1.pdf";


	StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;
	String^ fileFullName = "1.pdf";
	StorageFolder^ m_pdfFolder = KnownFolders::PicturesLibrary;
	task<StorageFile^>(m_pdfFolder->GetFileAsync(fileFullName)).then([this]
		(task<StorageFile^ >file)
	{
		try
		{
			StorageFile^ _pdfFile = file.get();

			auto ls = _pdfFile->Path->Data();
			FILE* pFile = NULL;
			HRESULT hr = _wfopen_s(&pFile, ls, L"rb");

			if (_pdfFile != nullptr)
			{
				create_task(_pdfFile->OpenAsync(FileAccessMode::Read)).then([this, file](task<IRandomAccessStream^> task)
				{
					try
					{
						IRandomAccessStream^ readStream = task.get();
						UINT64 const size = readStream->Size;
						if (size <= MAXUINT32)
						{
							DataReader^ dataReader = ref new DataReader(readStream);
							//dataReader->UnicodeEncoding = UnicodeEncoding::Utf8;
							create_task(dataReader->LoadAsync(static_cast<UINT32>(size))).then([this, file, dataReader](unsigned int numBytesLoaded)
							{
								Platform::Array<unsigned char>^ content = ref new Array < unsigned char >(numBytesLoaded);
								dataReader->ReadBytes(content);
								//renderPDFPages();
								HMODULE temu = LoadPackagedLibrary(L"newapi.dll", 0);
								int err = GetLastError();
								void* funcAddress = GetProcAddress(temu, "PDF_RenderCompare_Create_NewAPI");

								DLLFUN  Dll_clFun_New = (DLLFUN)funcAddress;
								CPDF_RenderCompare_NewAPI* m_newAPI;
								m_newAPI = (CPDF_RenderCompare_NewAPI*)Dll_clFun_New();

								FXQA_FILESTREAM* qamem = new FXQA_FILESTREAM;
								qamem->data = content->Data;
								qamem->len = content->Length;
								int errret = -1;

								

								//m_newAPI->LoadPDFFile(qamem, errret);
								m_newAPI->LoadPDFFile((FX_LPCWSTR)L"C:\\Users\\xiaoxia\\AppData\\Local\\Packages\\069c94df-2c92-4d83-97d6-2dad9f36099c_td1h08zsn3e00\\TempState\\1.pdf", errret);
								err = GetLastError();

								delete dataReader;
							});
						}
						else
						{
							delete readStream; // As a best practice, explicitly close the readStream resource as soon as it is no longer needed.
						}
					}
					catch (COMException^ ex)
					{
					}
				});
				//task<DocHandle>(Document::LoadAsync(_pdfFile, "")).then([this, module, tester]
				//	(task<DocHandle> docHandle)
				//{
				//	//m_hCurPDFDoc = docHandle.get();
				//	
				//});
			}

		}
		catch (Platform::Exception^ e)
		{

		}
	});

	
	int s = 0;
	s++;
}
