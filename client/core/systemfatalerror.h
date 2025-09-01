#ifndef SYSTEMFATALERROR_H
#define SYSTEMFATALERROR_H

#include <QString>

class SystemFatalError
{
public:
    static void show(const QString &message);
};


#endif // SYSTEMFATALERROR_H
