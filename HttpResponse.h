#ifndef __HTTPRESPONSE_H__
#define __HTTPRESPONSE_H__
#include <muduo/base/copyable.h>
#include <iostream>
#include <string>
#include <map>
#include <muduo/net/Buffer.h>
using namespace std;
class HttpResponse:public muduo::copyable
{
    public:
	enum HttpStatusCode
	{
	    CODE_UNKNOWN,
	    CODE_200=200,
	    CODE_301=301,
	    CODE_400=400,
	    CODE_404=404,
	    CODE_501=501
	};
	explicit HttpResponse(bool close)
	    : statusCode_(CODE_UNKNOWN),
	    closeConnection_(close),
	    cgi_(0)
    {
    }
	void setStatusCode(HttpStatusCode code);
	void setStatusMessage(const string&message);
	void setCloseConnection(bool on);
	bool closeConnection()const;
	void setContentType(const string&contentType);
	void addHeader(const string&key,const string&value);
	void setBody(const string &body);
	void appendToBuffer(muduo::net::Buffer*output,int cgi)const;
	void setCgi(int c);
	int cgi() const;
    private:
	//响应头
	std::map<string,string>headers_;
	//响应码
	HttpStatusCode statusCode_;
	//状态信息
	string statusMessage_;
	//是否 keep-alive
	bool closeConnection_;
	//响应报文
	string body_;
	//if cgi response
	int cgi_;

};

#endif
