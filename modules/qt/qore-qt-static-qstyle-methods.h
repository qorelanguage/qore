
#include "qore-qt-static-qobject-methods.h"

#if 0
class T {
#endif

   public:

      // these functions will never be called
      DLLLOCAL virtual int layoutSpacingImplementation ( QSizePolicy::ControlType control1, QSizePolicy::ControlType control2, Qt::Orientation orientation, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { return 0; }
      DLLLOCAL virtual QIcon standardIconImplementation ( QStyle::StandardPixmap standardIcon, const QStyleOption * option = 0, const QWidget * widget = 0 ) const { return QIcon(); }

#if 0
};
#endif
