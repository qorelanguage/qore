
#include "qore-qt-static-qobject-methods.h"

#if 0
class T {
#endif

   public:
      DLLLOCAL virtual void actionEvent(QActionEvent * event) { }
      DLLLOCAL virtual void changeEvent(QEvent * event) {}
      DLLLOCAL virtual void closeEvent(QCloseEvent * event) {}
      DLLLOCAL virtual void contextMenuEvent ( QContextMenuEvent * event ) {}
      DLLLOCAL virtual void dragEnterEvent ( QDragEnterEvent * event ) {}	
      DLLLOCAL virtual void dragLeaveEvent ( QDragLeaveEvent * event ) {}	
      DLLLOCAL virtual void dragMoveEvent ( QDragMoveEvent * event )  {}	
      DLLLOCAL virtual void dropEvent ( QDropEvent * event ) {}		
      DLLLOCAL virtual void enterEvent ( QEvent * event ) {}
      DLLLOCAL virtual void focusInEvent ( QFocusEvent * event ) {}	
      DLLLOCAL virtual void focusOutEvent ( QFocusEvent * event ) {}	
      DLLLOCAL virtual void hideEvent ( QHideEvent * event ) {}		
      DLLLOCAL virtual void inputMethodEvent ( QInputMethodEvent * event ) {} 
      DLLLOCAL virtual void keyPressEvent ( QKeyEvent * event ) {}		
      DLLLOCAL virtual void keyReleaseEvent ( QKeyEvent * event ) {}	
      DLLLOCAL virtual void leaveEvent ( QEvent * event ) {}		
      //DLLLOCAL virtual bool macEvent ( EventHandlerCallRef caller, EventRef event ) {}
      DLLLOCAL virtual void mouseDoubleClickEvent ( QMouseEvent * event ) {} 
      DLLLOCAL virtual void mouseMoveEvent ( QMouseEvent * event ) {}	
      DLLLOCAL virtual void mousePressEvent ( QMouseEvent * event ) {}	
      DLLLOCAL virtual void mouseReleaseEvent ( QMouseEvent * event ) {}	
      DLLLOCAL virtual void moveEvent ( QMoveEvent * event ) {}		
      DLLLOCAL virtual void paintEvent ( QPaintEvent * event ) {}		
      //DLLLOCAL virtual bool qwsEvent ( QWSEvent * event ) { return false; }
      DLLLOCAL virtual void resizeEvent ( QResizeEvent * event ) {}	
      DLLLOCAL virtual void showEvent ( QShowEvent * event ) {}		
      DLLLOCAL virtual void tabletEvent ( QTabletEvent * event ) {}	
      DLLLOCAL virtual void wheelEvent ( QWheelEvent * event ) {}		
      //DLLLOCAL virtual bool winEvent ( MSG * message, long * result ) { return false;}
      //DLLLOCAL virtual bool x11Event ( XEvent * event ) { return false; }
      //DLLLOCAL virtual HDC getDC () const {}
      DLLLOCAL virtual int heightForWidth ( int w ) const { return 0; }
      DLLLOCAL virtual QVariant inputMethodQuery ( Qt::InputMethodQuery query ) const { return QVariant(); }
      DLLLOCAL virtual QSize minimumSizeHint () const { return QSize(); }
      //DLLLOCAL virtual QPaintEngine * paintEngine () const { return 0; }
      //DLLLOCAL virtual void releaseDC ( HDC hdc ) const {}
      DLLLOCAL virtual void setVisible ( bool visible ) {}
      DLLLOCAL virtual QSize sizeHint() const { return QSize(); }

#if 0
};
#endif

