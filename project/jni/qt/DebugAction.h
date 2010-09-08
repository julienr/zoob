#ifndef ZOOB_DEBUG_ACTION_H
#define ZOOB_DEBUG_ACTION_H

#include "app.h"
#include <QAction>

//Specializsed action class that will toggle a debug flag
class DebugAction : public QAction {
  Q_OBJECT
  public:
    DebugAction(const QString& text, QObject* parent, eDebug what);

  signals:
    void debugChanged (eDebug what, bool enabled);

  public slots:
    void onChanged (bool checked);

  private:
    const eDebug what; //the debug flag toggled by this action
};

#endif
