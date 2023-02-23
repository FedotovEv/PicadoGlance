#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "14";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2023";
	static const char UBUNTU_VERSION_STYLE[] =  "23.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 5;
	static const long BUILD  = 5;
	static const long REVISION  = 95;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 669;
	#define RC_FILEVERSION 1,5,5,95
	#define RC_FILEVERSION_STRING "1, 5, 5, 95\0"
	static const char FULLVERSION_STRING [] = "1.5.5.95";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 13;
	

}
#endif //VERSION_H
