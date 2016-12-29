#include "HttpServer.h"

#if 1
void defaultHttpCallback(const HttpRequest&, HttpResponse* resp)
{
#if 1
    resp->setStatusCode(HttpResponse::CODE_400);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
#endif
}
#endif


#if 1
//构造函数
HttpServer::HttpServer(muduo::net::EventLoop* loop,
	const muduo::net::InetAddress& listenAddr)
    : server_(loop, listenAddr, "MyHttpd"),httpCallback_(defaultHttpCallback)
{
    server_.setConnectionCallback(
	    boost::bind(&HttpServer::onConnection, this, _1));

    server_.setMessageCallback(
	    boost::bind(&HttpServer::onMessage, this, _1, _2, _3));
}



HttpServer::~HttpServer()
{
}

void HttpServer::start()
{
    LOG_WARN << "HttpServer[" << server_.name()
	<< "] starts listenning on " << server_.ipPort();
    server_.start();
}
#endif


void HttpServer::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if (conn->connected())
    {
	conn->setContext(HttpContext());
    }
}

void HttpServer::onMessage(const muduo::net::TcpConnectionPtr& conn,
	muduo::net::Buffer* buf,
	muduo::Timestamp receiveTime)
{
    HttpContext*context=boost::any_cast<HttpContext>(conn->getMutableContext());
    if(!context->parseRequest(buf,receiveTime))
    {
	conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
	conn->shutdown();
    }
    if (context->gotAll())//state_==gotALL
    {
	//请求已经解析完毕
	onRequest(conn, context->request());

	context->reset();//context reset
    }

}

#if 1
void HttpServer::onRequest(const muduo::net::TcpConnectionPtr& conn, const HttpRequest& req)
{
#if 1
    const string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
	(req.getVersion() == HttpRequest::HTTP10 && connection != "Keep-Alive");
    HttpResponse response(close);//构造响应

    httpCallback_(req, &response);//用户回调
    muduo::net::Buffer buf;
    //此时response已经构造好，将向客户发送Response
    response.appendToBuffer(&buf,response.cgi());
    conn->send(&buf);
    //如果非Keep-Alive则直接关掉
    if (response.closeConnection())
    {
	conn->shutdown();
    }
#endif
}
#endif


