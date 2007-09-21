
#include "qore-qt-static-qobject-methods.h"

#if 0
class T {
#endif

   public:
      DLLLOCAL virtual void beginInsertColumns ( const QModelIndex & parent, int first, int last ) {}
      DLLLOCAL virtual void beginInsertRows ( const QModelIndex & parent, int first, int last ) {}
      DLLLOCAL virtual void beginRemoveColumns ( const QModelIndex & parent, int first, int last ) {}
      DLLLOCAL virtual void beginRemoveRows ( const QModelIndex & parent, int first, int last )  {}
      DLLLOCAL virtual void changePersistentIndex ( const QModelIndex & from, const QModelIndex & to ) {} 
      DLLLOCAL virtual void changePersistentIndexList ( const QModelIndexList & from, const QModelIndexList & to ) {} 
      DLLLOCAL virtual QModelIndex createIndex ( int row, int column, void * ptr = 0 ) const { return QModelIndex(); }
      DLLLOCAL virtual QModelIndex createIndex ( int row, int column, quint32 id ) const { return QModelIndex(); }
      DLLLOCAL virtual void endInsertColumns () {}
      DLLLOCAL virtual void endInsertRows () {}
      DLLLOCAL virtual void endRemoveColumns () {}
      DLLLOCAL virtual void endRemoveRows () {}
      DLLLOCAL virtual QModelIndexList persistentIndexList () const { return QModelIndexList(); }
      DLLLOCAL virtual void reset () {}


#if 0
};
#endif

