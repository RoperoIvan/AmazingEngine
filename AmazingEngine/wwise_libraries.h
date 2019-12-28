#ifndef _WWISE_LIBRARIES_H_
#define _WWISE_LIBRARIES_H_

#include "Wwise/include/AK/SoundEngine/Common/AkSoundEngine.h"   
#include "Wwise/include/AK/MusicEngine/Common/AkMusicEngine.h"   
#include "Wwise/include/AK/SoundEngine/Common/AkMemoryMgr.h"      
#include "Wwise/include/AK/SoundEngine/Common/AkModule.h"         
#include "Wwise/include/AK/SoundEngine/Common/AkStreamMgrModule.h"  
#include "Wwise/include/AK/SoundEngine/Common/AkTypes.h"
#include "Wwise/include/AK/Tools/Common/AkPlatformFuncs.h" 

/*	Uncomenting this define will set the library to "RELEASE" mode. Some chunks of code will be skipped
	to increase performance, but debug will not be possible.	*/
	//#define AK_OPTIMIZED

#ifdef AK_OPTIMIZED

#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkSoundEngine.lib")
#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkMusicEngine.lib")
#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkMemoryMgr.lib")
#pragma comment( lib, "3DEngine/Audiokinetic/Release/lib/AkStreamMgr.lib")

#else

#include "Wwise/include/AK/Comm/AkCommunication.h"
#pragma comment( lib, "Wwise/Debug/lib/CommunicationCentral.lib")
#pragma comment( lib, "Wwise/ws2_32.lib")
#pragma comment( lib, "Wwise/Debug/lib/AkSoundEngine.lib")
#pragma comment( lib, "Wwise/Debug/lib/AkMusicEngine.lib")
#pragma comment( lib, "Wwise/Debug/lib/AkMemoryMgr.lib")
#pragma comment( lib, "Wwise/Debug/lib/AkStreamMgr.lib")

#endif

#pragma comment( lib, "Wwise/dinput8.lib") 
#pragma comment( lib, "Wwise/dsound.lib")  
#pragma comment( lib, "Wwise/dxguid.lib")


namespace AK
{
#ifdef WIN32

	void* AllocHook(size_t in_size)
	{
		return malloc(in_size);
	}
	void FreeHook(void* in_ptr)
	{
		free(in_ptr);
	}
	// Note: VirtualAllocHook() may be used by I/O pools of the default implementation
	// of the Stream Manager, to allow "true" unbuffered I/O (using FILE_FLAG_NO_BUFFERING
	// - refer to the Windows SDK documentation for more details). This is NOT mandatory;
	// you may implement it with a simple malloc().
	void* VirtualAllocHook(void* in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect)
	{
		return VirtualAlloc(in_pMemAddress, in_size, in_dwAllocationType, in_dwProtect);
	}
	void VirtualFreeHook(void* in_pMemAddress, size_t in_size, DWORD in_dwFreeType)
	{
		VirtualFree(in_pMemAddress, in_size, in_dwFreeType);
	}
#endif // WIN32
}
#endif
