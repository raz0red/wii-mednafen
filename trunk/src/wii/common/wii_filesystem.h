//Stolen from http://code.google.com/p/dop-mii

#ifndef _FILESYSTEM_H_
#define _FILESYSTEM_H_

namespace IO 
{
	class SD
	{
	public:
		static bool Mount();
		static void Unmount();
	};

	class USB
	{
	private:
		static bool isMounted;
	public:
		static void Startup();
		static void Shutdown();
		static bool Mount();
		static void Unmount();
	};
};

#endif
