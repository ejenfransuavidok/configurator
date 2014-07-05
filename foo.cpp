#include "foo.h"
#include<QDebug>

static int counter = 0;

Foo::Foo()
{
    cnt = counter++;
    qDebug() << "constructor counter=" << cnt;
}

Foo::~Foo()
{
    qDebug() << "destructor " << cnt;
}

/*Foo& Foo::operator =(Foo*)
{

}*/
