#include "core/errorcode.h"
#include "utils/logging.h"

QString ErrorCode::decode(uint32_t code) {
    uint8_t aa = (code >> 24) & 0xFF;
    uint8_t bb = (code >> 16) & 0xFF;
    uint8_t cc = (code >> 8) & 0xFF;
    uint8_t dd = code & 0xFF;

    uint16_t key = (aa << 8) | bb;
    QString msg = errorMessages.value(key, "UnknownError");

    if (Logging::instance().getLogLevel() == Logging::Debug) {
        QString category = categoryNames.value(
            aa, QString("UnknownCategory(0x%1)").arg(aa, 2, 16, QChar('0')));
        QString module = moduleNames.value(
            cc, QString("UnknownModule(0x%1)").arg(cc, 2, 16, QChar('0')));

        return QString("[%1] [%2] %3 (DD=0x%4)")
            .arg(category, module, msg,
                 QString::number(dd, 16).rightJustified(2, '0'));
    } else {
        QString codeHex = QString("%1").arg(code, 8, 16, QChar('0')).toUpper();
        return QString("%1. (код ошибки: 0x%2)").arg(msg, codeHex);
    }
}
