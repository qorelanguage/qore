/*
 QC_QLineEdit.cc
 
 Qore Programming Language
 
 Copyright (C) 2003, 2004, 2005, 2006, 2007 David Nichols
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <qore/Qore.h>

#include "QC_QLineEdit.h"
#include "QC_QValidator.h"

int CID_QLINEEDIT;
class QoreClass *QC_QLineEdit = 0;

//QLineEdit ( QWidget * parent = 0 )
//QLineEdit ( const QString & contents, QWidget * parent = 0 )
static void QLINEEDIT_constructor(QoreObject *self, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   if (is_nothing(p)) {
      self->setPrivate(CID_QLINEEDIT, new QoreQLineEdit(self));
      return;
   }

   QString contents;
   bool got_string = !get_qstring(p, contents, xsink, true);
   if (got_string)
      p = get_param(params, 1);

   QoreAbstractQWidget *parent = (p && p->type == NT_OBJECT) ? (QoreAbstractQWidget *)p->val.object->getReferencedPrivateData(CID_QWIDGET, xsink) : 0;
   if (!parent && !is_nothing(p)) {
      xsink->raiseException("QLINEEDIT-CONSTRUCTOR-ERROR", "expecting [widget] or string, [parent widget] as arguments to QLineEdit::constructor(), got type '%s'", p->type->getName());
      return;
   }

   ReferenceHolder<QoreAbstractQWidget> parentHolder(parent, xsink);

   if (got_string)
      self->setPrivate(CID_QLINEEDIT, new QoreQLineEdit(self, contents, parent ? parent->getQWidget() : 0));
   else
      self->setPrivate(CID_QLINEEDIT, new QoreQLineEdit(self, parent ? parent->getQWidget() : 0));

   return;
}

static void QLINEEDIT_copy_method(class QoreObject *self, class QoreObject *old, class QoreQLineEdit *qle, ExceptionSink *xsink)
{
   xsink->raiseException("QLINEEDIT-COPY-ERROR", "objects of this class cannot be copied");
}

//Qt::Alignment alignment () const
static QoreNode *QLINEEDIT_alignment(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qle->qobj->alignment());
}

//void backspace ()
static QoreNode *QLINEEDIT_backspace(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->backspace();
   return 0;
}

////QCompleter * completer () const
//static QoreNode *QLINEEDIT_completer(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return qle->qobj->completer();
//}

//QMenu * createStandardContextMenu ()
static QoreNode *QLINEEDIT_createStandardContextMenu(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qm = new QoreObject(QC_QMenu, getProgram());
   QoreQMenu *q_qm = new QoreQMenu(o_qm, qle->qobj->createStandardContextMenu());
   o_qm->setPrivate(CID_QMENU, q_qm);
   return new QoreNode(o_qm);
}

//void cursorBackward ( bool mark, int steps = 1 )
static QoreNode *QLINEEDIT_cursorBackward(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool mark = p ? p->getAsBool() : false;
   p = get_param(params, 1);
   int steps = !is_nothing(p) ? p->getAsInt() : 1;
   qle->qobj->cursorBackward(mark, steps);
   return 0;
}

//void cursorForward ( bool mark, int steps = 1 )
static QoreNode *QLINEEDIT_cursorForward(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool mark = p ? p->getAsBool() : false;
   p = get_param(params, 1);
   int steps = !is_nothing(p) ? p->getAsInt() : 1;
   qle->qobj->cursorForward(mark, steps);
   return 0;
}

//int cursorPosition () const
static QoreNode *QLINEEDIT_cursorPosition(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qle->qobj->cursorPosition());
}

//int cursorPositionAt ( const QPoint & pos )
static QoreNode *QLINEEDIT_cursorPositionAt(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQPoint *pos = (p && p->type == NT_OBJECT) ? (QoreQPoint *)p->val.object->getReferencedPrivateData(CID_QPOINT, xsink) : 0;
   if (!pos) {
      if (!xsink->isException())
         xsink->raiseException("QLINEEDIT-CURSORPOSITIONAT-PARAM-ERROR", "expecting a QPoint object as first argument to QLineEdit::cursorPositionAt()");
      return 0;
   }
   ReferenceHolder<QoreQPoint> posHolder(pos, xsink);
   return new QoreNode((int64)qle->qobj->cursorPositionAt(*(static_cast<QPoint *>(pos))));
}

//void cursorWordBackward ( bool mark )
static QoreNode *QLINEEDIT_cursorWordBackward(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool mark = p ? p->getAsBool() : false;
   qle->qobj->cursorWordBackward(mark);
   return 0;
}

//void cursorWordForward ( bool mark )
static QoreNode *QLINEEDIT_cursorWordForward(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool mark = p ? p->getAsBool() : false;
   qle->qobj->cursorWordForward(mark);
   return 0;
}

//void del ()
static QoreNode *QLINEEDIT_del(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->del();
   return 0;
}

//void deselect ()
static QoreNode *QLINEEDIT_deselect(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->deselect();
   return 0;
}

//QString displayText () const
static QoreNode *QLINEEDIT_displayText(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qle->qobj->displayText().toUtf8().data(), QCS_UTF8));
}

//bool dragEnabled () const
static QoreNode *QLINEEDIT_dragEnabled(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->dragEnabled());
}

////EchoMode echoMode () const
//static QoreNode *QLINEEDIT_echoMode(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreNode((int64)qle->qobj->echoMode());
//}

//void end ( bool mark )
static QoreNode *QLINEEDIT_end(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool mark = p ? p->getAsBool() : false;
   qle->qobj->end(mark);
   return 0;
}

//bool hasAcceptableInput () const
static QoreNode *QLINEEDIT_hasAcceptableInput(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->hasAcceptableInput());
}

//bool hasFrame () const
static QoreNode *QLINEEDIT_hasFrame(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->hasFrame());
}

//bool hasSelectedText () const
static QoreNode *QLINEEDIT_hasSelectedText(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->hasSelectedText());
}

//void home ( bool mark )
static QoreNode *QLINEEDIT_home(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool mark = p ? p->getAsBool() : false;
   qle->qobj->home(mark);
   return 0;
}

//QString inputMask () const
static QoreNode *QLINEEDIT_inputMask(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qle->qobj->inputMask().toUtf8().data(), QCS_UTF8));
}

//void insert ( const QString & newText )
static QoreNode *QLINEEDIT_insert(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);

   QString newText;
   if (get_qstring(p, newText, xsink))
      return 0;

   qle->qobj->insert(newText);
   return 0;
}

//bool isModified () const
static QoreNode *QLINEEDIT_isModified(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->isModified());
}

//bool isReadOnly () const
static QoreNode *QLINEEDIT_isReadOnly(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->isReadOnly());
}

//bool isRedoAvailable () const
static QoreNode *QLINEEDIT_isRedoAvailable(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->isRedoAvailable());
}

//bool isUndoAvailable () const
static QoreNode *QLINEEDIT_isUndoAvailable(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(qle->qobj->isUndoAvailable());
}

//int maxLength () const
static QoreNode *QLINEEDIT_maxLength(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qle->qobj->maxLength());
}

/*
//virtual QSize minimumSizeHint () const
static QoreNode *QLINEEDIT_minimumSizeHint(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qle->qobj->minimumSizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}
*/

//QString selectedText () const
static QoreNode *QLINEEDIT_selectedText(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qle->qobj->selectedText().toUtf8().data(), QCS_UTF8));
}

//int selectionStart () const
static QoreNode *QLINEEDIT_selectionStart(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode((int64)qle->qobj->selectionStart());
}

//void setAlignment ( Qt::Alignment flag )
static QoreNode *QLINEEDIT_setAlignment(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   Qt::Alignment flag = (Qt::Alignment)(p ? p->getAsInt() : 0);
   qle->qobj->setAlignment(flag);
   return 0;
}

////void setCompleter ( QCompleter * c )
//static QoreNode *QLINEEDIT_setCompleter(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
//{
//   QoreNode *p = get_param(params, 0);
//   ??? QCompleter* c = p;
//   qle->qobj->setCompleter(c);
//   return 0;
//}

//void setCursorPosition ( int )
static QoreNode *QLINEEDIT_setCursorPosition(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qle->qobj->setCursorPosition(x);
   return 0;
}

//void setDragEnabled ( bool b )
static QoreNode *QLINEEDIT_setDragEnabled(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qle->qobj->setDragEnabled(b);
   return 0;
}

//void setEchoMode ( EchoMode )
static QoreNode *QLINEEDIT_setEchoMode(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QLineEdit::EchoMode echomode = (QLineEdit::EchoMode)(p ? p->getAsInt() : 0);
   qle->qobj->setEchoMode(echomode);
   return 0;
}

//void setFrame ( bool )
static QoreNode *QLINEEDIT_setFrame(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qle->qobj->setFrame(b);
   return 0;
}

//void setInputMask ( const QString & inputMask )
static QoreNode *QLINEEDIT_setInputMask(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString inputMask;

   if (get_qstring(p, inputMask, xsink))
      return 0;

   qle->qobj->setInputMask(inputMask);
   return 0;
}

//void setMaxLength ( int )
static QoreNode *QLINEEDIT_setMaxLength(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int x = p ? p->getAsInt() : 0;
   qle->qobj->setMaxLength(x);
   return 0;
}

//void setModified ( bool )
static QoreNode *QLINEEDIT_setModified(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qle->qobj->setModified(b);
   return 0;
}

//void setReadOnly ( bool )
static QoreNode *QLINEEDIT_setReadOnly(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool b = p ? p->getAsBool() : false;
   qle->qobj->setReadOnly(b);
   return 0;
}

//void setSelection ( int start, int length )
static QoreNode *QLINEEDIT_setSelection(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int start = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int length = p ? p->getAsInt() : 0;
   qle->qobj->setSelection(start, length);
   return 0;
}

//void setValidator ( const QValidator * v )
static QoreNode *QLINEEDIT_setValidator(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QoreQValidator *v = (p && p->type == NT_OBJECT) ? (QoreQValidator *)p->val.object->getReferencedPrivateData(CID_QVALIDATOR, xsink) : 0;
   if (*xsink)
      return 0;
   ReferenceHolder<AbstractPrivateData> vHolder(static_cast<AbstractPrivateData *>(v), xsink);
   qle->qobj->setValidator(v ? static_cast<QValidator *>(v->getQValidator()) : 0);
   return 0;
}

//virtual QSize sizeHint () const
static QoreNode *QLINEEDIT_sizeHint(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qs = new QoreObject(QC_QSize, getProgram());
   QoreQSize *q_qs = new QoreQSize(qle->sizeHint());
   o_qs->setPrivate(CID_QSIZE, q_qs);
   return new QoreNode(o_qs);
}

//QString text () const
static QoreNode *QLINEEDIT_text(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   return new QoreNode(new QoreString(qle->qobj->text().toUtf8().data(), QCS_UTF8));
}

//const QValidator * validator () const
static QoreNode *QLINEEDIT_validator(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   const QValidator *qt_qobj = qle->qobj->validator();
   if (!qt_qobj)
      return 0;
   QVariant qv_ptr = qt_qobj->property("qobject");
   QoreObject *rv_obj = reinterpret_cast<QoreObject *>(qv_ptr.toULongLong());
   if (rv_obj)
      rv_obj->ref();
   else {
      rv_obj = new QoreObject(QC_QValidator, getProgram());
      QoreQtQValidator *t_qobj = new QoreQtQValidator(rv_obj, const_cast<QValidator *>(qt_qobj));
      rv_obj->setPrivate(CID_QVALIDATOR, t_qobj);
   }
   return new QoreNode(rv_obj);
}

static QoreNode *QLINEEDIT_clear(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->clear();
   return 0;
}

//void copy () const
static QoreNode *QLINEEDIT_copy(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->copy();
   return 0;
}

//void cut ()
static QoreNode *QLINEEDIT_cut(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->cut();
   return 0;
}

//void paste ()
static QoreNode *QLINEEDIT_paste(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->paste();
   return 0;
}

//void redo ()
static QoreNode *QLINEEDIT_redo(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->redo();
   return 0;
}

//void selectAll ()
static QoreNode *QLINEEDIT_selectAll(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->selectAll();
   return 0;
}

//void setText ( const QString & )
static QoreNode *QLINEEDIT_setText(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   
   QString text;
   if (get_qstring(p, text, xsink))
      return 0;

   qle->qobj->setText(text);
   return 0;
}

//void undo ()
static QoreNode *QLINEEDIT_undo(QoreObject *self, QoreQLineEdit *qle, QoreNode *params, ExceptionSink *xsink)
{
   qle->qobj->undo();
   return 0;
}

QoreClass *initQLineEditClass(QoreClass *qwidget)
{
   QC_QLineEdit = new QoreClass("QLineEdit", QDOM_GUI);
   CID_QLINEEDIT = QC_QLineEdit->getID();

   QC_QLineEdit->addBuiltinVirtualBaseClass(qwidget);

   QC_QLineEdit->setConstructor(QLINEEDIT_constructor);
   QC_QLineEdit->setCopy((q_copy_t)QLINEEDIT_copy_method);

   QC_QLineEdit->addMethod("alignment",                   (q_method_t)QLINEEDIT_alignment);
   QC_QLineEdit->addMethod("backspace",                   (q_method_t)QLINEEDIT_backspace);
   //QC_QLineEdit->addMethod("completer",                   (q_method_t)QLINEEDIT_completer);
   QC_QLineEdit->addMethod("createStandardContextMenu",   (q_method_t)QLINEEDIT_createStandardContextMenu);
   QC_QLineEdit->addMethod("cursorBackward",              (q_method_t)QLINEEDIT_cursorBackward);
   QC_QLineEdit->addMethod("cursorForward",               (q_method_t)QLINEEDIT_cursorForward);
   QC_QLineEdit->addMethod("cursorPosition",              (q_method_t)QLINEEDIT_cursorPosition);
   QC_QLineEdit->addMethod("cursorPositionAt",            (q_method_t)QLINEEDIT_cursorPositionAt);
   QC_QLineEdit->addMethod("cursorWordBackward",          (q_method_t)QLINEEDIT_cursorWordBackward);
   QC_QLineEdit->addMethod("cursorWordForward",           (q_method_t)QLINEEDIT_cursorWordForward);
   QC_QLineEdit->addMethod("del",                         (q_method_t)QLINEEDIT_del);
   QC_QLineEdit->addMethod("deselect",                    (q_method_t)QLINEEDIT_deselect);
   QC_QLineEdit->addMethod("displayText",                 (q_method_t)QLINEEDIT_displayText);
   QC_QLineEdit->addMethod("dragEnabled",                 (q_method_t)QLINEEDIT_dragEnabled);
   //QC_QLineEdit->addMethod("echoMode",                    (q_method_t)QLINEEDIT_echoMode);
   QC_QLineEdit->addMethod("end",                         (q_method_t)QLINEEDIT_end);
   QC_QLineEdit->addMethod("hasAcceptableInput",          (q_method_t)QLINEEDIT_hasAcceptableInput);
   QC_QLineEdit->addMethod("hasFrame",                    (q_method_t)QLINEEDIT_hasFrame);
   QC_QLineEdit->addMethod("hasSelectedText",             (q_method_t)QLINEEDIT_hasSelectedText);
   QC_QLineEdit->addMethod("home",                        (q_method_t)QLINEEDIT_home);
   QC_QLineEdit->addMethod("inputMask",                   (q_method_t)QLINEEDIT_inputMask);
   QC_QLineEdit->addMethod("insert",                      (q_method_t)QLINEEDIT_insert);
   QC_QLineEdit->addMethod("isModified",                  (q_method_t)QLINEEDIT_isModified);
   QC_QLineEdit->addMethod("isReadOnly",                  (q_method_t)QLINEEDIT_isReadOnly);
   QC_QLineEdit->addMethod("isRedoAvailable",             (q_method_t)QLINEEDIT_isRedoAvailable);
   QC_QLineEdit->addMethod("isUndoAvailable",             (q_method_t)QLINEEDIT_isUndoAvailable);
   QC_QLineEdit->addMethod("maxLength",                   (q_method_t)QLINEEDIT_maxLength);
   //QC_QLineEdit->addMethod("minimumSizeHint",             (q_method_t)QLINEEDIT_minimumSizeHint);
   QC_QLineEdit->addMethod("selectedText",                (q_method_t)QLINEEDIT_selectedText);
   QC_QLineEdit->addMethod("selectionStart",              (q_method_t)QLINEEDIT_selectionStart);
   QC_QLineEdit->addMethod("setAlignment",                (q_method_t)QLINEEDIT_setAlignment);
   //QC_QLineEdit->addMethod("setCompleter",                (q_method_t)QLINEEDIT_setCompleter);
   QC_QLineEdit->addMethod("setCursorPosition",           (q_method_t)QLINEEDIT_setCursorPosition);
   QC_QLineEdit->addMethod("setDragEnabled",              (q_method_t)QLINEEDIT_setDragEnabled);
   QC_QLineEdit->addMethod("setEchoMode",                 (q_method_t)QLINEEDIT_setEchoMode);
   QC_QLineEdit->addMethod("setFrame",                    (q_method_t)QLINEEDIT_setFrame);
   QC_QLineEdit->addMethod("setInputMask",                (q_method_t)QLINEEDIT_setInputMask);
   QC_QLineEdit->addMethod("setMaxLength",                (q_method_t)QLINEEDIT_setMaxLength);
   QC_QLineEdit->addMethod("setModified",                 (q_method_t)QLINEEDIT_setModified);
   QC_QLineEdit->addMethod("setReadOnly",                 (q_method_t)QLINEEDIT_setReadOnly);
   QC_QLineEdit->addMethod("setSelection",                (q_method_t)QLINEEDIT_setSelection);
   QC_QLineEdit->addMethod("setValidator",                (q_method_t)QLINEEDIT_setValidator);
   QC_QLineEdit->addMethod("sizeHint",                    (q_method_t)QLINEEDIT_sizeHint);
   QC_QLineEdit->addMethod("text",                        (q_method_t)QLINEEDIT_text);
   QC_QLineEdit->addMethod("validator",                   (q_method_t)QLINEEDIT_validator);

   QC_QLineEdit->addMethod("clear",                       (q_method_t)QLINEEDIT_clear);
   QC_QLineEdit->addMethod("qt_copy",                     (q_method_t)QLINEEDIT_copy);
   QC_QLineEdit->addMethod("cut",                         (q_method_t)QLINEEDIT_cut);
   QC_QLineEdit->addMethod("paste",                       (q_method_t)QLINEEDIT_paste);
   QC_QLineEdit->addMethod("redo",                        (q_method_t)QLINEEDIT_redo);
   QC_QLineEdit->addMethod("selectAll",                   (q_method_t)QLINEEDIT_selectAll);
   QC_QLineEdit->addMethod("setText",                     (q_method_t)QLINEEDIT_setText);
   QC_QLineEdit->addMethod("undo",                        (q_method_t)QLINEEDIT_undo);

   return QC_QLineEdit;
}
