/***
 * The content of this file or document is CONFIDENTIAL and PROPRIETARY
 * to ChengZhen(Anyou).  It is subject to the terms of a
 * License Agreement between Licensee and ChengZhen(Anyou).
 * restricting among other things, the use, reproduction, distribution
 * and transfer.  Each of the embodiments, including this information and
 * any derivative work shall retain this copyright notice.
 *
 * Copyright (c) 2014-2015 ChengZhen(Anyou). All Rights Reserved.
 *
 */
#ifndef _NS_VISION_CORE_OS_FILE_H
#define _NS_VISION_CORE_OS_FILE_H

namespace core {
namespace os {


//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// FileStream class

/**
 * 文件流.
 */
class FileStream
{
public:
	FileStream();
	~FileStream();

public:
	String GetFileName();
	UINT GetLength();

	void SetFileName(LPCSTR filename);

public:
	BOOL Exists();
	BOOL CanWrite();

public:
	int Close();
	int Open(int mode);
	int Read(void *buf, size_t count);
	int Write(const void *buf, size_t count);

public:
	int SafeRead(void *buf, size_t count);
	int SafeWrite(const void *buf, size_t count);

public:
	String fFileName;
	int fFile;
};

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// File class

class File;

/** File 智能指针类型. */
typedef SmartPtr<File> FilePtr;

/** File 智能指针数组类型. */
typedef ArrayList<FilePtr> FileArray;

/** 
 * 提供用于创建、复制、删除、移动和打开文件的方法
 *
 * @author ChengZhen (anyou@msn.com)
 */
class File : public Object
{
public:
	/** FileFilter 代表一个文件查找过滤接口. */
	class FileFilter 
	{
	public:
		/**
		 * 指出指定的文件是否是可接受的.
		 * @param file 要检测的文件.
		 * @return 如果可接受则返回 TRUE. 
		 */
		virtual BOOL Accept(FilePtr file) { return TRUE; }
	};

public:
	File();
	File( LPCSTR filename );
	File( LPCSTR parent, LPCSTR child );

// Attributes -------------------------------------------------
public:
	BOOL   CanRead();
	BOOL   CanWrite();
	String GetAbsolutePath();
	String GetExtName();
	INT64  GetFreeSpace();
	UINT   GetMode();
	String GetName();	 
	String GetPath();	 
	String GetParent();
	time_t GetLastModified();
	INT64  GetTotalSpace();

	BOOL   IsDirectory();
	BOOL   IsFile();
	BOOL   IsPidExists(int* ppid = NULL);

	UINT   Length();
	void   SetPath(LPCSTR filename);

// Operations -------------------------------------------------
public:
	BOOL   Delete();
	BOOL   Exists();
	int    ListFiles( FileArray& files, FileFilter* filter = NULL );
	void   Merge(LPCSTR name);
	BOOL   Mkdir();
	BOOL   Mkdirs();
	String ReadString(UINT maxLength = 64 * 1024, UINT offset = 0);
	BOOL   RenameTo( LPCSTR filename );
	int    WriteString(LPCSTR content);

public:
	static int  MakeDirectory ( char *path, long mode, int flags );
	static char* MergePath( char *basePath, LPCSTR subPath );
	static int  Remove(LPCSTR path, int flags);

// Implementation ---------------------------------------------
protected:
	void Init();

// Data Members -----------------------------------------------
public:
	static const char PATH_SEPARATOR;	///< 与系统有关的默认路径分隔符字符。
	static const char SEPARATOR;		///< 与系统有关的默认名称分隔符。

private:
	String fPath;	///< 这个文件的路径
	String fName;	///< 这个文件的名称
};

}; // namespace os
}; // namespace core

#endif //

