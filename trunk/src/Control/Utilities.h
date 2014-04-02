///	\class		Utilities
/// \brief		Static class used for general functions for all other classes (Mostly writing to files)
///	\author		Andrew Thompson
///	\version	1.0
///	\date		June 4, 2008
/// \warning	Class under construction
///	bug			Add back-slash before the word bug and add a bug if one is found

#pragma once

class Utilities
{
public:
	Utilities(void);
	//static CArray<double, double> *LoadDataFromFile(char *filename);
	static bool WriteFile(CString fileName, CArray<double, double> * doubleCArrayData, long length=NULL, CString *warning=NULL);
	static bool WriteFile(CString fileName, double * doubleArrayData, const long count);
	static bool WriteFile(CString fileName, CArray<int, int> * intArrayData);
	static bool WriteFile(CString fileName, CArray<CString, CString> * CStringArrayData, long length=NULL, CString *warning=NULL);
	static bool WriteFile(CString fileName, CString SingleCStringData);
	static void WriteXuAngleFile(CString SingleCStringData, bool enabled);

	static void ReadDataFile(CString FilePath, CArray<CString> &CStringArrayData);
};
