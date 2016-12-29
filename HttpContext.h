#ifndef __HTTPCONTEXT_H__
#define __HTTPCONTEXT_H__

#include <muduo/base/copyable.h>
#include <muduo/net/Buffer.h>
#include "HttpRequest.h"
class Buffer;
class HttpContext:public muduo::copyable
{
    public:
	enum HttpRequestParseState
	{
	    kExpectRequestLine,
	    kExpectHeaders,
	    kExpectBody,
	    kGotAll,
	};

	HttpContext()
	    : state_(kExpectRequestLine)
	{
	}

	bool parseRequest(muduo::net::Buffer* buf, muduo::Timestamp receiveTime);

	bool gotAll() const
	{ return state_ == kGotAll; }

	void reset()
	{
	    state_ = kExpectRequestLine;
	    HttpRequest dummy;
	    request_.swap(dummy);
	}
	const HttpRequest& request() const
	{ return request_; }

	HttpRequest& request()
	{ return request_; }

    private:
	bool processRequestLine(const char* begin, const char* end);

	HttpRequestParseState state_;
	HttpRequest request_;
};
#endif

