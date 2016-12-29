#ifndef __HTTPSERVER_H__
#define __HTTPSERVER_H__
#include <muduo/base/Logging.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <iostream>
#include <string>
#include <muduo/net/Buffer.h>
#include "HttpContext.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
using namespace std;


class HttpServer : boost::noncopyable
{
    public:
	//http回调类型
	typedef boost::function<void (const HttpRequest&,
		HttpResponse*)> HttpCallback;

	//构造函数
	HttpServer(muduo::net::EventLoop* loop,
		const muduo::net::InetAddress& listenAddr);

	~HttpServer();  // force out-line dtor, for scoped_ptr members.

	muduo::net::EventLoop* getLoop() const { return server_.getLoop(); }

	void setHttpCallback(const HttpCallback& cb)
	{
	    httpCallback_ = cb;
	}

	void setThreadNum(int numThreads)
	{
	    server_.setThreadNum(numThreads);
	}


	void start();



    private:
	//onConnection
	void onConnection(const muduo::net::TcpConnectionPtr &conn);
	//onMessage
	void onMessage(const muduo::net::TcpConnectionPtr&conn,
		muduo::net::Buffer*buf,
		muduo::Timestamp receiveTime);
	//+++++
	void onRequest(const muduo::net::TcpConnectionPtr&,const HttpRequest&);

	muduo::net::TcpServer server_;
	HttpCallback httpCallback_;
};
#endif
