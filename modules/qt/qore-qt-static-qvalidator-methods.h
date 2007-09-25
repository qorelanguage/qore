
#include "qore-qt-static-qobject-methods.h"

#if 0
class T {
#endif

   public:

   DLLLOCAL virtual void fixup(QString & input) const 
   { 
      qobj->fixup(input); 
   }
   DLLLOCAL virtual QValidator::State validate(QString & input, int & pos) const 
   {
#ifdef _QORE_QT_QC_QVALIDATOR_H
      return QValidator::Invalid;
#else
      return qobj->validate(input, pos); 
#endif
   }

#if 0
};
#endif
