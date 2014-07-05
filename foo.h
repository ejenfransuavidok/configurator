#ifndef FOO_H
#define FOO_H

#include<QObject>

class Foo : public QObject
{
    Q_OBJECT
public:
    Foo();
    ~Foo();
    //Foo& operator =(Foo*);
private:
    int cnt;
};

#endif // FOO_H
