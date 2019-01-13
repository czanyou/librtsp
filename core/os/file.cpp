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
#include "stdafx.h"

#include "core/os/file.h"

namespace core {
namespace os { 

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// FileStream class

FileStream::FileStream()
{
	fFile = -1;
}

FileStream::~FileStream()
{
	Close();
}

BOOL FileStream::CanWrite()
{
	// 检测文件是否存在
	if (access(fFileName, W_OK) == 0) {
		return 1;
	}

	return 0;
}

int FileStream::Close()
{
	if (fFile > 0) {
		close(fFile);
		fFile = -1;
	}

	return 0;
}


/** 测试此路径名表示的文件或目录是否存在。*/
BOOL FileStream::Exists()
{
	return access(fFileName, R_OK) == 0;
}

String FileStream::GetFileName()
{
	return fFileName;
}

/** 返回由此路径名表示的文件的长度。*/
UINT FileStream::GetLength()
{
	struct stat st;
	if (stat(fFileName, &st) < 0) {
		return 0;
	}

	return (UINT)st.st_size;
}

int FileStream::Open( int mode )
{
	Close();

	if (mode & O_CREAT) {
		fFile = open(fFileName, mode, 0666);

	} else {
		fFile = open(fFileName, mode);
	}

	if (fFile <= 0) {
		fFile = -1;
		return -1;
	}

	return 0;
}

int FileStream::Read( void *buf, size_t count )
{
	char* leftData = (char*)buf;
	int	  leftSize = count;

	int totalRead = 0;
	while (leftSize > 0) {
		int cc = SafeRead(leftData, leftSize);
		if (cc < 0) {
			return cc;

		} else if (cc == 0) {
			break;
		}

		totalRead += cc;
		leftData  += cc;
		leftSize  -= cc;
	}

	return totalRead;
}

int FileStream::SafeRead(void *buf, size_t count)
{
	if (fFileName <= 0) {
		return -1;
	}

	int n = 0;
	do {
		n = read(fFile, buf, count);
	} while (n < 0 && errno == EINTR);
	return n;
}

int FileStream::SafeWrite(const void *buf, size_t count)
{
	if (fFileName <= 0) {
		return -1;
	}

	int n = 0;
	do {
		n = write(fFile, buf, count);
	} while (n < 0 && errno == EINTR);
	return n;
}

void FileStream::SetFileName( LPCSTR filename )
{
	fFileName = filename ? filename : "";
}

int FileStream::Write( const void *buf, size_t count )
{
	LPCSTR	leftData = (LPCSTR)buf;
	int		leftSize = count;

	int totalWrite = 0;
	while (leftSize > 0) {
		int cc = SafeWrite(leftData, leftSize);
		if (cc < 0) {
			return cc;

		} else if (cc == 0) {
			break;
		}

		totalWrite += cc;
		leftData += cc;
		leftSize -= cc;
	}

	return totalWrite;
}

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// File class

const char File::PATH_SEPARATOR = ':';

#ifdef _WIN32
const char File::SEPARATOR = '\\';
#else
const char File::SEPARATOR = '/';
#endif


/** 默认构建方法. */
File::File()
{

}

/**
 * 根据指定的路径名构建一个新的 File 实例.
 * @param pathname 路径名字符串.
 */
File::File( LPCSTR pathname )
{
	if (pathname) {
		fPath = pathname;
	}

	fPath = fPath.Trim();
	Init();
}


/**
 * 根据父路径名和子路径名构建一个新的 File 实例.
 * @param parent 父路径字符串.
 * @param child 子路径字符串.
 */
File::File( LPCSTR parent, LPCSTR child )
{
	char pathname[MAX_PATH + 1];
	memset(pathname, 0, sizeof(pathname));
	if (parent) {
		strncpy(pathname, parent, MAX_PATH);
	}
	fPath = pathname;
	fPath = fPath.Trim();

	if (child) {
		Merge(child);

	} else {
		Init();
	}
}

/** 测试应用程序是否可以读取此路径名表示的文件。*/
BOOL File::CanRead()
{
	return access(fPath.c_str(), R_OK) == 0;
}

/** 测试应用程序是否可以修改此路径名表示的文件。*/
BOOL File::CanWrite()
{
	return access(fPath.c_str(), W_OK) == 0;
}

/** 删除此路径名表示的文件或目录。*/
BOOL File::Delete()
{
	Remove(fPath.c_str(), 1);
	return access(fPath.c_str(), R_OK) != 0;
}

/** 测试此路径名表示的文件或目录是否存在。*/
BOOL File::Exists()
{
	return access(fPath.c_str(), R_OK) == 0;
}

/** 返回此路径名的绝对路径名形式字符串。*/
String File::GetAbsolutePath()
{
	char filename[MAX_PATH + 1];
	memset(filename, 0, sizeof(filename));

#ifndef _WIN32
	if (realpath(fPath, filename)) {
		return filename;
	} else {
		return fPath;
	}

#else 
	::GetFullPathNameA(fPath.c_str(), MAX_PATH, filename, NULL);
	//LOG_D("2 %s, %s\r\n", fPath.c_str(), filename);
	return filename;
#endif

}

/** 返回这个文件的扩展名. */
String File::GetExtName()
{
	char* p = strrchr((char*)fName.c_str(), '.');
	return p ? p + 1 : "";
}

INT64 File::GetFreeSpace()
{
	if (fPath.IsEmpty()) {
		return 0;
	}

#ifdef __linux
	struct statfs fs;
	if (statfs(fPath, &fs) == 0) {
		INT64 freeSize = fs.f_bfree;
		freeSize *= fs.f_bsize;
		return freeSize;
	}

#endif
	return 0;
}

/** 返回此路径名表示的文件最后一次被修改的时间。*/
time_t File::GetLastModified()
{
	struct stat st;
	if (stat(fPath.c_str(), &st) >= 0) {
		return st.st_mtime;
	}
	return 0;
}

/** 返回这个文件的所有模式信息. */
UINT File::GetMode()
{
	struct stat st;
	if (stat(fPath.c_str(), &st) >= 0) {
		return st.st_mode;
	}
	return 0;
}

///< 返回这个文件的名称
String File::GetName()
{
	return fName;
}

/** 返回此路径名的父路径名的路径名字符串，如果此路径名没有指定父目录，则返回 ""。*/
String File::GetParent()
{
	String path = GetAbsolutePath();
	while (path.EndsWith("/")) {
		path = path.SubString(0, path.GetLength() - 1);
	}

	LPCSTR str = path.c_str();
	LPCSTR p = strrchr(str, SEPARATOR);
	if (p == NULL) {
		return "/";
	}
	return String(str, p - str);
}

///< 返回这个文件的路径
String File::GetPath()
{
	return fPath;
}

INT64 File::GetTotalSpace()
{
	if (fPath.IsEmpty()) {
		return 0;
	}

#ifdef __linux
	struct statfs fs;
	if (statfs(fPath, &fs) == 0) {
		INT64 totalSize = fs.f_blocks;
		totalSize *= fs.f_bsize;
		return totalSize;
	}
#endif
	return 0;
}

/** 返回由此路径名表示的文件的长度。*/
UINT File::Length()
{
	struct stat st;
	if (stat(fPath.c_str(), &st) < 0) {
		return 0;
	}

	return (UINT)st.st_size;
}

/**
 * 返回一个路径名数组，这些路径名表示此路径名所表示目录中的文件。
 * @param files 输出参数, 保存列出的所有路径名.
 * @param filter 过滤器, 如果为 NULL 则返回所有文件.
 * @return 返回找到的文件的个数.
 */
int File::ListFiles( FileArray& files, FileFilter* filter )
{
	int count = 0;

	struct dirent **ent = NULL;
	int total = scandir(fPath.c_str(), &ent, NULL, alphasort);
	if (total < 0) {
		return 0;
	}

	char fullname[MAX_PATH + 1];
	memset(fullname, 0, sizeof(fullname));

	for (int i = 0; i < total; i++) {
		if (strcmp(ent[i]->d_name, ".") == 0) {
			continue;

		} else if (strcmp(ent[i]->d_name, "..") == 0) {
			continue;
		}

		strncpy(fullname, fPath.c_str(), MAX_PATH);
		MergePath(fullname, ent[i]->d_name);
		FilePtr file = new File(fullname);
		if (filter == NULL || filter->Accept(file)) {
			files.Add(file);
			count++;
		}
	}
	return count;
}

/** 修正路径的格式. */
void File::Init()
{
	fName = "";
#ifndef _WIN32
	fPath.Replace('\\', SEPARATOR);
#else 
	fPath.Replace('/', SEPARATOR);
#endif

	int len = fPath.GetLength();
	if (len <= 0) {
		return;
	}

	LPCSTR p = fPath.c_str();
	LPCSTR s = p + len;
	while (s > p && *s != SEPARATOR) {
		s--;
	}

	if (*s == SEPARATOR) {
		s++;
	}
	fName = s;
}

/** 测试此路径名表示的文件是否是一个标准文件。*/
BOOL File::IsFile()
{
	struct stat st;
	if (stat(fPath.c_str(), &st) >= 0) {
		return !S_ISDIR(st.st_mode);
	}
	return FALSE;
}

/** 测试此路径名表示的文件是否是一个目录。*/
BOOL File::IsDirectory()
{
	struct stat st;
	if (stat(fPath.c_str(), &st) >= 0) {
		return S_ISDIR(st.st_mode);
	}
	return FALSE;
}

BOOL File::IsPidExists(int* ppid)
{
	if (!Exists()) {
		return FALSE;
	}

	String content = ReadString(MAX_PATH);
	content = content.Trim();
	int pid = atoi(content.c_str());
	if (pid <= 0) {
		return FALSE;
	}

	if (ppid) {
		*ppid = pid;
	}

	String path;
	path.Format("/proc/%d", pid);
	File proc(path);
	return proc.Exists();
}

/**
 * 创建此路径名指定的目录。
 * @param path 要创建的路径.
 * @param mode 创建模式.
 * @param flags 创建标志参数.
 * @return 如果成功则返回 0. 
 */
int File::MakeDirectory (char *path, long mode, int flags)
{
	//LPCSTR fail_msg = NULL;
	char *s = path;
	struct stat st;

#ifndef _WIN32
	mode_t mask;

	mask = umask(0);
	if (mode == -1) {
		umask(mask);
		mode = (S_IXUSR | S_IXGRP | S_IXOTH | S_IWUSR | S_IWGRP | S_IWOTH |
			S_IRUSR | S_IRGRP | S_IROTH) & ~mask;
	} else {
		umask(mask & ~0300);
	}
#endif

	do {
		char c = 0;

		if (flags & 0x01) {	/* Get the parent. */
			/* Bypass leading non-'/'s and then subsequent '/'s. */
			while (*s) {
				if (*s == SEPARATOR) {
					do {
						++s;
					} while (*s == SEPARATOR);
					c = *s;		/* Save the current char */
					*s = 0;		/* and replace it with null. */
					break;
				}
				++s;
			}
		}

		if (mkdir(path, 0777) < 0) {
			/* If we failed for any other reason than the directory
			* already exists, output a diagnostic and return -1.*/
			if (errno != EEXIST	|| !(flags & 0x01)
				|| (stat(path, &st) < 0 || !S_ISDIR(st.st_mode))) {
					//fail_msg = "create";
					umask(mask);
					break;
			}
			/* Since the directory exists, don't attempt to change
			* permissions if it was the full target.  Note that
			* this is not an error conditon. */
			if (!c) {
				umask(mask);
				return 0;
			}
		}

		if (!c) {
			/* Done.  If necessary, updated perms on the newly
			* created directory.  Failure to update here _is_
			* an error.*/
			umask(mask);
			if ((mode != -1) && (chmod(path, mode) < 0)) {
				//fail_msg = "set permissions of";
				break;
			}
			return 0;
		}

		/* Remove any inserted null from the path (recursive mode). */
		*s = c;

	} while (1);

	return -1;
}

/** 设置路径. */
void File::Merge( LPCSTR childPath )
{
	if (childPath) {
		String path = childPath;
		if (path.GetLength() > MAX_PATH) {
			path = path.SubString(0, MAX_PATH);
		}

		fPath += SEPARATOR;
		fPath += path.Trim();
		Init();
	}
}

/**
 * 根据父路径名和子路径名创建新的路径名.
 * @param basePath 父路径名字符串
 * @param childPath 子路径名字符串
 * @return 返回新的路径名.
 */ 
char* File::MergePath(char *basePath, LPCSTR childPath)
{
	if (basePath == NULL || isempty(childPath)) {
		return basePath;
	} 

	int baseLen = strlen(basePath);
	if (baseLen == 0) {
		strncpy(basePath, childPath, MAX_PATH);
		return basePath;
	}

	// 在基路径尾添加 '/' 分隔符.
	if (basePath[baseLen - 1] != SEPARATOR) {
		basePath[baseLen++] = SEPARATOR;
		basePath[baseLen]   = '\0';
	}

	// 跳过子路径开始的 '/' 符号
	LPCSTR s = childPath;
	while (*s == SEPARATOR) {
		s++;
	}

	// 合并成完整的路径
	strncpy(basePath + baseLen, s, MAX_PATH - baseLen);   
	return basePath;
}

/** 创建此路径名指定的目录。*/
BOOL File::Mkdir()
{
	char pathname[MAX_PATH + 1];
	memset(pathname, 0, sizeof(pathname));
	strncpy(pathname, fPath.c_str(), MAX_PATH);
	return MakeDirectory(pathname, 0777, 0) == 0;
}

/** 创建此路径名指定的目录，包括创建必需但不存在的父目录。*/
BOOL File::Mkdirs()
{
	char pathname[MAX_PATH + 1];
	memset(pathname, 0, sizeof(pathname));
	strncpy(pathname, fPath.c_str(), MAX_PATH);

	return MakeDirectory(pathname, 0777, 1) == 0;
}

/** 
 * 读取由此路径名表示的文件的文本内容. 
 * 注意不适合读取非文本文件和超过 64K 的文本文件. 
 */
String File::ReadString(UINT maxLength, UINT offset)
{
	String content;
	FILE* file = fopen(GetAbsolutePath(), "r");
	if (file == NULL) {
		return content;
	}

	if (offset > 0) {
		fseek(file, offset, SEEK_SET);
	}

	char buffer[1025];
	memset(buffer, 0, sizeof(buffer));

	UINT fileSize = 0;
	while (fileSize < maxLength) {
		int readSize = fread(buffer, 1, 1024, file);
		if (readSize <= 0) {
			if (ferror(file) && (errno == EINTR)) {
				continue;
			}
			break;
		}

		buffer[readSize] = '\0';
		fileSize += readSize;
		content += buffer;
	}

	fclose(file);
	return content;
}

/**
 * 删除指定的路径.
 * @param path 要删除的路径.
 * @param flags 删除标志参数.
 * @return 如果成功则返回 0. 
 */
int File::Remove(LPCSTR path, int flags)
{
	if (isempty(path)) {
		return 0;
	}

	struct stat path_stat;
	int path_exists = 1;
	char full_path[MAX_PATH + 1];
	memset(full_path, 0, sizeof(full_path));

	if (lstat(path, &path_stat) < 0) {
		if (errno != ENOENT) {
			return -1;
		}

		path_exists = 0;
	}

	if (!path_exists) {
		return 0;
	}

	if (S_ISDIR(path_stat.st_mode)) {
		DIR *dp;
		struct dirent *d;
		int status = 0;

		if ((dp = opendir(path)) == NULL) {
			return -1;
		}

		while ((d = readdir(dp)) != NULL) {
			strncpy(full_path, path, MAX_PATH);
			if (isequal(d->d_name, ".") || isequal(d->d_name, "..")) {
				continue;
			}

			char* new_path = File::MergePath(full_path, d->d_name);
			if (new_path == NULL) {
				continue;
			}

			if (Remove(new_path, flags) < 0) {
				status = -1;
			}
		}

		if (closedir(dp) < 0) {
			return -1;
		}

		if (rmdir(path) < 0) {
			LOG_W("unable to remove `%s'\r\n", path);
			return -1;
		}

		return status;

	} else {

		if (unlink(path) < 0) {
			LOG_W("unable to remove `%s'\r\n", path);
			return -1;
		}

		return 0;
	}

	return 0;
}

/**
 * 重新命名此路径名表示的文件。
 * @param pathname 指定文件的新路径名
 * @return 命名成功时返回 TRUE, 否则返回 FALSE. 
 */
BOOL File::RenameTo( LPCSTR pathname )
{
	if (isempty(pathname)) {
		return FALSE;
	}
	return rename(fPath.c_str(), pathname) == 0;
}

/** 设置路径. */
void File::SetPath( LPCSTR pathname )
{
	fPath = pathname ? pathname : "";
	Init();
}

int File::WriteString(LPCSTR content)
{
	if (content == NULL) {
		return 0;
	}

	String filename = GetAbsolutePath();
	String tmpname = filename;
	tmpname += "-";

	FileStream fileStream;
	fileStream.SetFileName(tmpname);
	if (fileStream.Open(O_WRONLY | O_CREAT) < 0) {
		return 0;
	}

	int leftSize = strlen(content);
	int totalSize = fileStream.Write(content, leftSize);
	fileStream.Close();

	if (totalSize == leftSize) {
		remove(filename);
		rename(tmpname, filename);
		return totalSize;

	} else {
		remove(tmpname);
		return 0;
	}
}

}; // namespace os
}; // namespace core

