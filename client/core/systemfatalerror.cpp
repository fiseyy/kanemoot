#include "core/systemfatalerror.h"
#include <QCoreApplication>
#if defined(Q_OS_WIN)
#include <windows.h>
#elif defined(Q_OS_LINUX)
#include <gtk/gtk.h>
#endif

void SystemFatalError::show(const QString &message)
{
#if defined(Q_OS_WIN)
    MessageBoxW(nullptr,
                reinterpret_cast<const wchar_t*>(message.utf16()),
                L"Fatal Error",
                MB_ICONERROR | MB_OK);
#elif defined(Q_OS_MACOS)
    MacOSError::showError(message);
#elif defined(Q_OS_LINUX)
    gtk_init(0, nullptr);
    GtkWidget *dialog = gtk_message_dialog_new(nullptr,
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_OK,
                                               "%s",
                                               message.toUtf8().constData());
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
#else
    // fallback: вывод в консоль
    qWarning("Fatal Error: %s", qPrintable(message));
#endif

    QCoreApplication::exit(EXIT_FAILURE);
}
