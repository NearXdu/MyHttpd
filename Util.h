#ifndef __UTIL_H__
#define __UTIL_H__
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <vector>
#include <sstream>
using namespace std;
namespace Util
{
    string ConstructPath(const string &path);
    string GetExtent(const string &path);
    void GetContentType(const string&,string&);
    string GetContent(const string &);
    string GetImage(const string &);
}
#endif
