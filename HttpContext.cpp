#include "HttpContext.h"
//解析请求头
bool HttpContext::processRequestLine(const char*begin,const char*end)
{
    bool succeed=false;
    const char*start=begin;
    const char*space = std::find(start,end,' ');
    //设置请求方法//method_
    if(space!=end&&request_.setMethod(start,space))
    {
	start=space+1;
	space = std::find(start,end,' ');
	if(space !=end)
	{
	    //解析URI
	    const char* question = std::find(start, space, '?');
	    if (question != space)
	    {
		request_.setPath(start, question);
		request_.setQuery(question, space);
	    }
	    else
	    {
		request_.setPath(start, space);
	    }
	    //解析HTTP版本号
	    start = space+1;
	    succeed = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
	    if (succeed)
	    {
		if (*(end-1) == '1')
		{
		    request_.setVersion(HttpRequest::HTTP11);
		}
		else if (*(end-1) == '0')
		{
		    request_.setVersion(HttpRequest::HTTP10);
		}
		else
		{
		    succeed = false;
		}
	    }//endif


	}//endif

    }//endif
    return succeed;
}

//解析请求头
bool HttpContext::parseRequest(muduo::net::Buffer*buf,muduo::Timestamp receiveTime)
{
    bool ok= true;
    bool hasMore=true;
    while(hasMore)
    {
	//解析请求行
	if(state_==kExpectRequestLine)
	{
	    //
	    const char*crlf=buf->findCRLF();
	    if(crlf)
	    {
		//开始解析请求行
		ok=processRequestLine(buf->peek(),crlf);
		if(ok)
		{
		    //解析成功
		    request_.setReceiveTime(receiveTime);
		    //回收请求行buffer
		    buf->retrieveUntil(crlf+2);
		    state_=kExpectHeaders;
		}
		else
		{
		    hasMore=false;
		}
	    }
	    else
	    {
		hasMore=false;
	    }
	}
	//解析请求头
	else if(state_==kExpectHeaders)
	{
	    const char* crlf = buf->findCRLF();

	    if (crlf)
	    {
		const char* colon = std::find(buf->peek(), crlf, ':');

		if (colon != crlf)
		{
		    //构造请求头
		    request_.addHeader(buf->peek(), colon, crlf);
		}
		else
		{
		    // empty line, end of header
		    // FIXME:
		    state_ = kExpectBody;
			
#if 0
		    state_=kGotAll;
		    hasMore = false;
#endif
		}
		buf->retrieveUntil(crlf + 2);//回收
	    }
	    else
	    {
		hasMore = false;
	    }
	}
	else if(state_==kExpectBody)
	{
	    request_.setBody(buf->peek(),buf->peek()+buf->readableBytes());
	    buf->retrieveAll();
	    hasMore=false;
	    state_=kGotAll;
	    // FIXME:
	}
    }//endwhile

    return ok;

}
