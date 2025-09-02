#include "core/systemfatalerror.h"
#import <Cocoa/Cocoa.h>

void MacOSError::showError(const QString &message){
        @autoreleasepool {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert setMessageText:@"Fatal Error"];
            [alert setInformativeText:[NSString stringWithUTF8String:message.toUtf8().constData()]];
            [alert addButtonWithTitle:@"OK"];
            [alert runModal];
        }
 }
