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

#include "nio_channel.h"
#include "nio_selector.h"

namespace core {
namespace net {

//_____________________________________________________________________________
///////////////////////////////////////////////////////////////////////////////
// Pipe class 

const UINT kPipeStdIn	= 0;
const UINT kPipeStdOut	= 1;


/** 默认构建方法. */
SocketPipe::SocketPipe()
{
	fPipes[kPipeStdIn]  = -1;
	fPipes[kPipeStdOut] = -1;
}

SocketPipe::~SocketPipe()
{
	Close();
}

/** 打开这个管道. */
int SocketPipe::Open()
{
	if (fPipes[kPipeStdIn] > 0) {
		return 0;
	}

	if (pipe(fPipes) < 0) {
		return -1;
	}

	SocketSelector::SetNoBlock(fPipes[kPipeStdIn], TRUE);
	SocketSelector::SetNoBlock(fPipes[kPipeStdOut], TRUE);
	return 0;
}

/** 关闭这个管道. */
int SocketPipe::Close()
{
	if (fPipes[kPipeStdIn] > 0) {
		close(fPipes[kPipeStdIn]);
		fPipes[kPipeStdIn]  = -1;
	}

	if (fPipes[kPipeStdOut] > 0) {
		close(fPipes[kPipeStdOut]);
		fPipes[kPipeStdOut] = -1;
	}

	return 0;
}

/** 返回输入管道的文件描述符. */
int& SocketPipe::InPipe()
{
	return fPipes[kPipeStdIn];
}

/** 返回输出管道的文件描述符. */
int& SocketPipe::OutPipe()
{
	return fPipes[kPipeStdOut];
}

} //
}
