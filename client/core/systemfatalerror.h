#ifndef SYSTEMFATALERROR_H
#define SYSTEMFATALERROR_H

#include <QString>

class SystemFatalError
{
public:
    static void show(const QString &message);
};

#if defined(Q_OS_MACOS)
class MacOSError {
public:
    static void showError(const QString &message);
};
#endif


#endif // SYSTEMFATALERROR_H
