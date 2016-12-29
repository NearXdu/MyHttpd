#include "HttpServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <muduo/net/EventLoop.h>
#include <muduo/base/Logging.h>
#include "Util.h"
#include <sys/types.h>
#include <sys/wait.h>


#include <iostream>
#include <map>
#include <string>
using namespace std;


void onRequest(const HttpRequest& req, HttpResponse* resp)
{

    int cgi=0;//on - off
    if(req.method()!=HttpRequest::GET&&req.method()!=HttpRequest::POST)
    {
	resp->setStatusCode(HttpResponse::CODE_501);
	resp->setStatusMessage("Method Not Implemented");
	resp->setContentType("text/html");
	resp->setBody("<HTML><HEAD><TITLE>Method Not Implemented</TITLE></HEAD><BODY><P>HTTP request method not supported.</BODY></HTML>");
	return ;
    }
    if(req.method()==HttpRequest::POST||
	   (req.method()==HttpRequest::GET&&!req.query().empty()) )
    {
	cgi=1;
	resp->setCgi(cgi);
    }
    if(!cgi)
    {
	string path=Util::ConstructPath(req.path());
	string extent=Util::GetExtent(path);
	string contentType="";
	Util::GetContentType(extent,contentType);
	string content=Util::GetContent(path);
	if(content.empty())
	{
	    resp->setStatusCode(HttpResponse::CODE_404);
	    resp->setStatusMessage("Not Found");
	}
	else
	{
	    resp->setBody(content);
	    resp->setStatusCode(HttpResponse::CODE_200);
	    resp->setStatusMessage("OK");
	    resp->setContentType(contentType);
	} 
	return ;
    }
    //CGI SCRIPT
#if 1
    int cgi_output[2];
    int cgi_input[2];
    pid_t pid;
    int status;
    char c;
    //POST
    if(req.method()==HttpRequest::POST)
    {
//	cout<<req.getBody()<<endl;
//	cout<<"xxxx"<<req.getHeader("Content-Length")<<endl;
	
	int contentLengthValue=std::stoi(req.getHeader("Content-Length"));
	string databody=req.getBody();
	resp->setStatusCode(HttpResponse::CODE_200);
	resp->setStatusMessage("OK");
	if(pipe(cgi_output)<0)
	{
	    printf("pipe error\r\n");
	    return ;
	}
	if(pipe(cgi_input)<0)
	{
	    printf("pipe error\r\n");
	    return ;
	}
	if ( (pid = fork()) < 0 )
	{
	    printf("fork error\r\n");
	    return;
	}
	if(pid==0)
	{
	    char meth_env[255];
	    char length_env[255];

	    string Path(req.path().begin()+1,req.path().end());

	    sprintf(meth_env, "REQUEST_METHOD=%s", req.methodString());
	    sprintf(length_env, "CONTENT_LENGTH=%d", contentLengthValue);

	    putenv(meth_env);
	    putenv(length_env);

	    dup2(cgi_output[1], 1);
	    dup2(cgi_input[0], 0);
	    close(cgi_output[0]);
	    close(cgi_input[1]);
	    
	    execl(Path.c_str(), Path.c_str(), NULL);
	    exit(0);
	}
	else
	{

	    string cgibody;
	    close(cgi_output[1]);
	    close(cgi_input[0]);

	    for(int i=0;i<databody.size();++i)
	    {
		write(cgi_input[1],&databody[i] ,1 );
	    }
//	    write(cgi_input[1],databody.c_str() ,databody.size() );
#if 1
	    while (read(cgi_output[0], &c, 1) > 0)
	    {
		cgibody+=c;
	    }
#endif

	    close(cgi_output[0]);
	    close(cgi_input[1]);
	    resp->setBody(cgibody);
	    waitpid(pid, &status, 0);

	}

    }//POST END
    else if(req.method()==HttpRequest::GET)//GET
    {
	resp->setStatusCode(HttpResponse::CODE_200);
	resp->setStatusMessage("OK");
	if(pipe(cgi_output)<0)
	{
	    printf("pipe error\r\n");
	    return ;
	}
	if(pipe(cgi_input)<0)
	{
	    printf("pipe error\r\n");
	    return ;
	}
	if ( (pid = fork()) < 0 )
	{
	    printf("fork error\r\n");
	    return;
	}

	if(pid==0)
	{
	    char meth_env[255];

	    char query_env[255];

	    string Path(req.path().begin()+1,req.path().end());
	    string Query(req.query().begin()+1,req.query().end());
	    sprintf(meth_env, "REQUEST_METHOD=%s", req.methodString());

	    sprintf(query_env, "QUERY_STRING=%s", Query.c_str());

	    dup2(cgi_output[1], 1);
	    dup2(cgi_input[0], 0);
	    close(cgi_output[0]);
	    close(cgi_input[1]);
	    putenv(meth_env);
	    putenv(query_env);

	    execl(Path.c_str(), Path.c_str(), NULL);
	    exit(0);
	}
	else
	{
	    string cgibody;
	    close(cgi_output[1]);
	    close(cgi_input[0]);
#if 1
	    while (read(cgi_output[0], &c, 1) > 0)
	    {
		cgibody+=c;
	    }
#endif

	    close(cgi_output[0]);
	    close(cgi_input[1]);
	    resp->setBody(cgibody);
	    waitpid(pid, &status, 0);
	}

    }//GET END
    else
    {
	// Bad Request!
	//FIXME:
	return ;
    }
#endif
    //cgi
}

int main(int argc, char* argv[])
{
    int numThreads = 0;
    if (argc > 1)
    {
	//benchmark = true;
	muduo::Logger::setLogLevel(muduo::Logger::WARN);
	numThreads = atoi(argv[1]);
    }
    muduo::net::EventLoop loop;
    HttpServer server(&loop, muduo::net::InetAddress(8000) );
    server.setHttpCallback(onRequest);
    server.setThreadNum(numThreads);
    server.start();
    loop.loop();
    return 0;
}


