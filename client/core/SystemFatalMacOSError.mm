#if defined(Q_OS_MACOS)
#import <Cocoa/Cocoa.h>

class MacOSError {
public:
    static void showError(const QString &message){
        @autoreleasepool {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setMessageText:@"Fatal Error"];
            [alert setInformativeText:[NSString stringWithUTF8String:message.toUtf8().constData()]];
            [alert addButtonWithTitle:@"OK"];
            [alert runModal];
        }
    }
};

#endif
