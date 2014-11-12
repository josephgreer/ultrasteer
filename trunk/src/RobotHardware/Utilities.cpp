#include "Utilities.h"
#include <string.h>
#include <fstream>

Utilities::Utilities(void)
{
}


/*
/// \brief		Load contents of file into CArray object
///	\pre		File must exist and must have entry on each new line
///	\post		File is loaded into CArray object
CArray<double, double> *Utilities::LoadDataFromFile(char *filename)
{
	CArray<double, double> *data = new CArray<double, double>();
	
	string full_path = "data\\" + filename + ".txt";
	CStdioFile fin;
	if( !fin.Open(full_path.c_str(), CFile::modeRead) ) 
	{
		//AfxMessageBox(_T("data\\noise.txt file does not exist, cannot continue"));
		TRACE("%s file does not exist", full_path); 
		ASSERT(0);
		return NULL;
	}

	CString str;
	int i = 0;
	
	while( fin.ReadString(str) ) { i++; }
	noise->SetSize(i);

	fin.Seek( 0, CFile::begin );

	i=0;
	while( fin.ReadString(str) ) {
		noise->SetAt(i++, atof( str ));
	}

	return noise;
}
*/


/// \brief		Write double floating point CArray contents to disk
///	\pre		CArray is valid data and so is the filename valid for the OS
///	\post		All data in double CArray is written to disk with the given filename
bool Utilities::WriteFile(CString fileName, CArray<double, double> * doubleCArrayData, long length/*=NULL*/, CString *warning/*=NULL*/)
{
	if(doubleCArrayData == NULL || doubleCArrayData->GetCount() == 0) 
	{
		AfxMessageBox( "ERROR: Array is empty, build array first before writing file" );
		ASSERT(0);
		return false;
	}

	if(length)
	{
		if(length > doubleCArrayData->GetCount())
		{
			AfxMessageBox("ERROR: Input parameter 'length' is greater than the count of the array");
			ASSERT(0);
			return false;
		}
	}

	CFile file(fileName, CFile::modeCreate | CFile::modeWrite);

	CString str;
	
	long num;
	if(length) num = length;
	else num  = (long)doubleCArrayData->GetCount();
	
	int i=0;
	for(; i < num; i++) 
	{
		str.Format("%e\r\n", doubleCArrayData->GetAt(i));
		file.Write((LPCTSTR)str, str.GetLength());
	}

	file.Close();

	TRACE( "Utilities : Writing file %s to disk with %d values\n", fileName, num );

	if(i == num) return true;
	else return false;
}


/// \brief		Write double floating point array contents to disk
///	\pre		Array is valid data and so is the filename valid for the OS
///	\post		All data in array is written to disk with the given filename
bool Utilities::WriteFile(CString fileName, double * doubleArrayData, const long count)
{
	if(doubleArrayData == NULL || count == 0) {
		AfxMessageBox( "ERROR: Array is empty, build array first before writing file" );
		ASSERT(0);
		return false;
	}

	CFile file(fileName, CFile::modeCreate | CFile::modeWrite);

	CString str;
	int i=0;
	for(; i < count; i++) 
	{
		str.Format("%e\r\n", doubleArrayData[i]);
		file.Write((LPCTSTR)str, str.GetLength());
	}

	file.Close();

	TRACE( "Utilities : Writing file %s to disk with %d values\n", fileName, count );

	if(i == count) return true;
	else return false;
}


/// \brief		Write integar array contents to disk
///	\pre		array is valid data and so is the filename valid for the OS
///	\post		All data in array is written to disk with the given filename
bool Utilities::WriteFile(CString fileName, CArray<int, int> * intArrayData)
{
	if(intArrayData == NULL || intArrayData->GetCount() == 0) {
		AfxMessageBox( "ERROR: Array is empty, build array first before writing file" );
		ASSERT(0);
		return false;
	}

	CFile file(fileName, CFile::modeCreate | CFile::modeWrite);

	CString str;
	long num  = (long)intArrayData->GetCount();
	int i=0;
	for(; i < num; i++) 
	{
		str.Format("%d\r\n", intArrayData->GetAt(i));
		file.Write((LPCTSTR)str, str.GetLength());
	}

	file.Close();

	TRACE( "Utilities : Writing file %s to disk with %d values\n", fileName, num );

	if(i == num) return true;
	else return false;
}


/// \brief		Write CString array contents to disk, or if length is set only write that many elements to disk
///	\details	NOTE: If length is not NULL it should be no larger than the count of the array. If warning is 
///				not NULL it should be a pointer to a string (without line termination characters)
///	\pre		Array is valid data and so is the filename valid for the OS
///	\post		All data in array is written to disk with the given filename. Warning may also be included
///				at the start of the file
bool Utilities::WriteFile(CString fileName, CArray<CString, CString> * CStringArrayData, long length/*=NULL*/, CString *warning/*=NULL*/)
{
	if(CStringArrayData == NULL || CStringArrayData->GetCount() == 0) {
		AfxMessageBox( "ERROR: Array is empty, build array first before writing file" );
		ASSERT(0);
		return false;
	}

	if(length)
	{
		if(length > CStringArrayData->GetCount())
		{
			AfxMessageBox("ERROR: Input parameter 'length' is greater than the count of the array");
			ASSERT(0);
			return false;
		}
	}

	CFile file(fileName, CFile::modeCreate | CFile::modeWrite);

	CString str;
	
	if(warning)
	{
		*warning += "\r\n";
		file.Write( (LPCTSTR)*warning, warning->GetLength() );
	}

	long num;
	if(length) num = length;
	else num  = (long)CStringArrayData->GetCount();

	int i=0;
	for(; i < num; i++) 
	{
		str = CStringArrayData->GetAt(i);
		str += "\r\n";
		file.Write( (LPCTSTR)str, str.GetLength() );
	}

	file.Close();

	TRACE( "Utilities : Writing file %s to disk with %d values\n", fileName, num );

	if(i == num) return true;
	else return false;
}


/// \brief		Write single CString to disk (with flushing)
///	\pre		CString is valid data and so is the filename valid for the OS
///	\post		A single Cstring is written to disk
bool Utilities::WriteFile(CString fileName, CString SingleCStringData)
{
	CFile file(fileName, CFile::modeCreate | CFile::modeWrite);

	file.Write((LPCTSTR)SingleCStringData, SingleCStringData.GetLength());
	
	file.Flush();

	file.Close();

	TRACE( "Utilities : Writing file %s to disk with single CString value\n", fileName );

	return true;
}


/// \brief		Write necessary data to Xu's angle file
///	\pre		Write privileges to disk
///	\post		0 or 1 is written to first line of file and then either some string is written to indicate
///				why the first line was 0 or alternatively the angle is written to the file
void Utilities::WriteXuAngleFile(CString SingleCStringData, bool enabled)
{
	static CString dataToWrite;
	dataToWrite.Format("%d\r\n%s", (int)enabled , (LPCTSTR)SingleCStringData);

	CFile file("data\\XuRollAngle.txt", CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone);

	file.Write((LPCTSTR)dataToWrite, dataToWrite.GetLength());

	file.Close();

	TRACE( "Utilities : Writing Xu's data to disk\n" );
}



void Utilities::ReadDataFile(CString FilePath, CArray<CString> &CStringArrayData)
{
	try
	{
		std::fstream file_op(FilePath, std::ios::in);
	
		char file_data[1000];
		int i=0;
		while(!file_op.eof())
        {
              file_op.getline(file_data,1000);
			  CStringArrayData.Add(file_data);
			  i++;
        }
		file_op.close();
	}

	catch(char * str )
	{
		AfxMessageBox("Error: Reading File");
	}
	

}