#ifndef MD5THREAD_H
#define MD5THREAD_H
#include <string>
#include <boost/thread.hpp>
#include <openssl/md5.h>
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <stdlib.h>
#include <QMessageBox>

using namespace std;
using namespace boost;
/*
class md5thread
{
public:
    md5thread();
    virtual void fileToMd5(string){}GS

};
*/
void  fileToMd5Thread(string filepath,string classname,void* voidparentthread);

#endif // MD5THREAD_H
