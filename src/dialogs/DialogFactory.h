#ifndef DIALOGFACTORY_H
#define DIALOGFACTORY_H

#include "../auxiliary/CypressConstants.h"

#include <QObject>
#include <QSharedPointer>
#include <QMap>

QT_FORWARD_DECLARE_CLASS(DialogBase)

class DialogFactory
{
public:
    static DialogFactory *instance();
    ~DialogFactory();

    DialogBase* instantiate(const CypressConstants::Type&);
    DialogBase* instantiate(const QString&);

private:
    DialogFactory() = default;
    static DialogFactory *pInstance;
};

#endif // DIALOGFACTORY_H