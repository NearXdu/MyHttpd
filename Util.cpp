#include "Util.h"
namespace Util
{
    //获取请求的绝对路径
    string ConstructPath(const string &path)
    {
	string webHome="/home/zhangxiao/zxtest/myhttpd/httpservermuduo";
	if(path=="/")
	{
	    return webHome+"/index.html";
	}
	else
	{
	    return webHome+path;
	}
    }

    //获取文件扩展名
    string GetExtent(const string &path)
    {
	int i;
	for(i=path.size()-1;;--i)
	{
	    if(path[i]=='.')
		break;
	}
	return string(path.begin()+i+1,path.end());
    }
    //获取content-Type
    void GetContentType(const string &tmpExtension,
	    string &contentType)
    {
	ifstream mimeFile("mime.types");
	string line;
	while(NULL!=getline(mimeFile,line))
	{
	    if(line[0]!='#')
	    {
		stringstream lineStream(line);
		contentType.clear();
		lineStream>>contentType;
		vector<string>extensions;
		string extension;
		while(lineStream>>extension)
		{
		    extensions.push_back(extension);
		}
		for(int i=0;i<extensions.size();++i)
		{
		    if(tmpExtension==extensions[i])
		    {
			mimeFile.close();
			return ;
		    }
		}

	    }
	}
	//如果都不匹配就默认为text/plain
	contentType="text/plain";
	mimeFile.close();
    }
    //获取Content
    string GetContent(const string &fileName)
    {
	std::ifstream fin(fileName, std::ios::in | std::ios::binary);
	if(fin.fail())
	{
	    return string("");
	}
	std::ostringstream oss;
	oss << fin.rdbuf();
	return std::string(oss.str());
    }
}
