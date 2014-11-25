
#include <collection.h>
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Foundation::Collections;
using namespace Platform;

namespace help
{
	IVectorView<StorageFile^>^ addivec(IVectorView<StorageFile^>^ ziel,IVectorView<StorageFile^>^ add);
	IVectorView<StorageFile^>^ addiIvec(IVectorView<IStorageItem^>^ adda,IVectorView<StorageFile^>^ addb);
	IVectorView<StorageFile^>^ getSupportedFiles(IVectorView<StorageFile^>^ Files);
	String^ timetostring(Windows::Foundation::TimeSpan time);
	String^ timetostring(__int64 time);
	__int64 timetosec(Windows::Foundation::TimeSpan time);
	int64 sectotime(__int64);
	FileOpenPicker^ create_open_picker();
	FileOpenPicker^ create_open_picker(String^ Openname);
	FolderPicker^ create_folder_picker(String^ Openname);
	bool suported(String^ filetype);

	Array<String^>^ getSupportedAudio();
	Array<String^>^ getSupportedVideo();
	Array<String^>^ getOtherSupportedFiles();
	Array<String^>^ getSupportedFiles();
}
namespace settings
{
	bool initSettings();
	String^ getValue(String^ key);
	int setValue(String^ key, String^ Value);
	bool reset();
}