#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "23";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2023";
	static const char UBUNTU_VERSION_STYLE[] =  "23.05";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 7;
	static const long BUILD  = 9;
	static const long REVISION  = 158;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 818;
	#define RC_FILEVERSION 1,7,9,158
	#define RC_FILEVERSION_STRING "1, 7, 9, 158\0"
	static const char FULLVERSION_STRING [] = "1.7.9.158";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 22;
	

}
#endif //VERSION_H
