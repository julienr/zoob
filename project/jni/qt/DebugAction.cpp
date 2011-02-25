#include "DebugAction.h"

DebugAction::DebugAction(const QString& text, QObject* parent, AppInterface::eDebug what)
  : QAction(text, parent),
  what(what) {
  setCheckable(true);
  connect(this, SIGNAL(toggled(bool)), this, SLOT(onChanged(bool)));
}

void DebugAction::onChanged (bool checked) {
  emit debugChanged(what, checked);
}


