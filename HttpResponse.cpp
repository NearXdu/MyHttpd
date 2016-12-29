#include "HttpResponse.h"
void HttpResponse::setStatusCode(HttpStatusCode code)
{
    statusCode_=code;
}
void HttpResponse::setStatusMessage(const string& message)
{
    statusMessage_ = message;
}
void HttpResponse::setCloseConnection(bool on)
{
    closeConnection_ = on;
}
bool HttpResponse::closeConnection() const
{
    return closeConnection_;
}
void HttpResponse::setContentType(const string& contentType)
{
    addHeader("Content-Type", contentType);
}
void HttpResponse::addHeader(const string& key, const string& value)
{
    headers_[key] = value;
}
void HttpResponse::setBody(const string& body)
{
    body_ = body;
}

void HttpResponse::setCgi(int c)
{
    cgi_=c;
}
int HttpResponse::cgi()const
{
    return cgi_;
}

void HttpResponse::appendToBuffer(muduo::net::Buffer* output,int cgi) const
{
    char buf[32];
    //构造响应行
    snprintf(buf, sizeof buf, "HTTP/1.1 %d ", statusCode_);
    output->append(buf);
    output->append(statusMessage_);
    output->append("\r\n");

    if (closeConnection_)
    {
	//
	output->append("Connection: close\r\n");
    }
    else
    {
	//Keep-Alive需要Content-Length
	snprintf(buf, sizeof buf, "Content-Length: %zd\r\n", body_.size());
	output->append(buf);
	output->append("Connection: Keep-Alive\r\n");
    }

    for (std::map<string, string>::const_iterator it = headers_.begin();
	    it != headers_.end();
	    ++it)
    {
	//迭代构造响应头
	output->append(it->first);
	output->append(": ");
	output->append(it->second);
	output->append("\r\n");
    }

    if(!cgi)
	output->append("\r\n");
    //响应报文
    output->append(body_);

}







