/*
 QC_QPrinter.cc
 
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

#include "QC_QPrinter.h"
#include "QC_QRect.h"

#include "qore-qt.h"

int CID_QPRINTER;
class QoreClass *QC_QPrinter = 0;

//QPrinter ( PrinterMode mode = ScreenResolution )
static void QPRINTER_constructor(QoreObject *self, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::PrinterMode mode = !is_nothing(p) ? (QPrinter::PrinterMode)p->getAsInt() : QPrinter::ScreenResolution;
   self->setPrivate(CID_QPRINTER, new QoreQPrinter(mode));
   return;
}

static void QPRINTER_copy(class QoreObject *self, class QoreObject *old, class QoreQPrinter *qp, ExceptionSink *xsink)
{
   xsink->raiseException("QPRINTER-COPY-ERROR", "objects of this class cannot be copied");
}

//bool abort ()
static QoreNode *QPRINTER_abort(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qp->abort());
}

//bool collateCopies () const
static QoreNode *QPRINTER_collateCopies(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qp->collateCopies());
}

//ColorMode colorMode () const
static QoreNode *QPRINTER_colorMode(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->colorMode());
}

//QString creator () const
static QoreNode *QPRINTER_creator(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qp->creator().toUtf8().data(), QCS_UTF8);
}

//QString docName () const
static QoreNode *QPRINTER_docName(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qp->docName().toUtf8().data(), QCS_UTF8);
}

//bool doubleSidedPrinting () const
static QoreNode *QPRINTER_doubleSidedPrinting(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qp->doubleSidedPrinting());
}

//bool fontEmbeddingEnabled () const
static QoreNode *QPRINTER_fontEmbeddingEnabled(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qp->fontEmbeddingEnabled());
}

//int fromPage () const
static QoreNode *QPRINTER_fromPage(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->fromPage());
}

//bool fullPage () const
static QoreNode *QPRINTER_fullPage(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qp->fullPage());
}

//bool newPage ()
static QoreNode *QPRINTER_newPage(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBoolNode(qp->newPage());
}

//int numCopies () const
static QoreNode *QPRINTER_numCopies(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->numCopies());
}

//Orientation orientation () const
static QoreNode *QPRINTER_orientation(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->orientation());
}

//QString outputFileName () const
static QoreNode *QPRINTER_outputFileName(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qp->outputFileName().toUtf8().data(), QCS_UTF8);
}

//OutputFormat outputFormat () const
static QoreNode *QPRINTER_outputFormat(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->outputFormat());
}

//PageOrder pageOrder () const
static QoreNode *QPRINTER_pageOrder(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->pageOrder());
}

//QRect pageRect () const
static QoreNode *QPRINTER_pageRect(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qp->pageRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//PageSize pageSize () const
static QoreNode *QPRINTER_pageSize(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->pageSize());
}

////virtual QPaintEngine * paintEngine () const
//static QoreNode *QPRINTER_paintEngine(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qp->paintEngine());
//}

//QRect paperRect () const
static QoreNode *QPRINTER_paperRect(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreObject *o_qr = new QoreObject(QC_QRect, getProgram());
   QoreQRect *q_qr = new QoreQRect(qp->paperRect());
   o_qr->setPrivate(CID_QRECT, q_qr);
   return o_qr;
}

//PaperSource paperSource () const
static QoreNode *QPRINTER_paperSource(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->paperSource());
}

////QPrintEngine * printEngine () const
//static QoreNode *QPRINTER_printEngine(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
//{
//   ??? return new QoreBigIntNode(qp->printEngine());
//}

//QString printProgram () const
static QoreNode *QPRINTER_printProgram(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qp->printProgram().toUtf8().data(), QCS_UTF8);
}

//PrintRange printRange () const
static QoreNode *QPRINTER_printRange(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->printRange());
}

//QString printerName () const
static QoreNode *QPRINTER_printerName(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qp->printerName().toUtf8().data(), QCS_UTF8);
}

//QString printerSelectionOption () const
static QoreNode *QPRINTER_printerSelectionOption(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreStringNode(qp->printerSelectionOption().toUtf8().data(), QCS_UTF8);
}

//PrinterState printerState () const
static QoreNode *QPRINTER_printerState(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->printerState());
}

//int resolution () const
static QoreNode *QPRINTER_resolution(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->resolution());
}

//void setCollateCopies ( bool collate )
static QoreNode *QPRINTER_setCollateCopies(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool collate = p ? p->getAsBool() : false;
   qp->setCollateCopies(collate);
   return 0;
}

//void setColorMode ( ColorMode newColorMode )
static QoreNode *QPRINTER_setColorMode(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::ColorMode newColorMode = (QPrinter::ColorMode)(p ? p->getAsInt() : 0);
   qp->setColorMode(newColorMode);
   return 0;
}

//void setCreator ( const QString & creator )
static QoreNode *QPRINTER_setCreator(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString creator;
   if (get_qstring(p, creator, xsink))
      return 0;
   qp->setCreator(creator);
   return 0;
}

//void setDocName ( const QString & name )
static QoreNode *QPRINTER_setDocName(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   qp->setDocName(name);
   return 0;
}

//void setDoubleSidedPrinting ( bool doubleSided )
static QoreNode *QPRINTER_setDoubleSidedPrinting(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool doubleSided = p ? p->getAsBool() : false;
   qp->setDoubleSidedPrinting(doubleSided);
   return 0;
}

//void setFontEmbeddingEnabled ( bool enable )
static QoreNode *QPRINTER_setFontEmbeddingEnabled(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool enable = p ? p->getAsBool() : false;
   qp->setFontEmbeddingEnabled(enable);
   return 0;
}

//void setFromTo ( int from, int to )
static QoreNode *QPRINTER_setFromTo(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int from = p ? p->getAsInt() : 0;
   p = get_param(params, 1);
   int to = p ? p->getAsInt() : 0;
   qp->setFromTo(from, to);
   return 0;
}

//void setFullPage ( bool fp )
static QoreNode *QPRINTER_setFullPage(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   bool fp = p ? p->getAsBool() : false;
   qp->setFullPage(fp);
   return 0;
}

//void setNumCopies ( int numCopies )
static QoreNode *QPRINTER_setNumCopies(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int numCopies = p ? p->getAsInt() : 0;
   qp->setNumCopies(numCopies);
   return 0;
}

//void setOrientation ( Orientation orientation )
static QoreNode *QPRINTER_setOrientation(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::Orientation orientation = (QPrinter::Orientation)(p ? p->getAsInt() : 0);
   qp->setOrientation(orientation);
   return 0;
}

//void setOutputFileName ( const QString & fileName )
static QoreNode *QPRINTER_setOutputFileName(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString fileName;
   if (get_qstring(p, fileName, xsink))
      return 0;
   qp->setOutputFileName(fileName);
   return 0;
}

//void setOutputFormat ( OutputFormat format )
static QoreNode *QPRINTER_setOutputFormat(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::OutputFormat format = (QPrinter::OutputFormat)(p ? p->getAsInt() : 0);
   qp->setOutputFormat(format);
   return 0;
}

//void setPageOrder ( PageOrder pageOrder )
static QoreNode *QPRINTER_setPageOrder(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::PageOrder pageOrder = (QPrinter::PageOrder)(p ? p->getAsInt() : 0);
   qp->setPageOrder(pageOrder);
   return 0;
}

//void setPageSize ( PageSize newPageSize )
static QoreNode *QPRINTER_setPageSize(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::PageSize newPageSize = (QPrinter::PageSize)(p ? p->getAsInt() : 0);
   qp->setPageSize(newPageSize);
   return 0;
}

//void setPaperSource ( PaperSource source )
static QoreNode *QPRINTER_setPaperSource(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::PaperSource source = (QPrinter::PaperSource)(p ? p->getAsInt() : 0);
   qp->setPaperSource(source);
   return 0;
}

//void setPrintProgram ( const QString & printProg )
static QoreNode *QPRINTER_setPrintProgram(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString printProg;
   if (get_qstring(p, printProg, xsink))
      return 0;
   qp->setPrintProgram(printProg);
   return 0;
}

//void setPrintRange ( PrintRange range )
static QoreNode *QPRINTER_setPrintRange(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QPrinter::PrintRange range = (QPrinter::PrintRange)(p ? p->getAsInt() : 0);
   qp->setPrintRange(range);
   return 0;
}

//void setPrinterName ( const QString & name )
static QoreNode *QPRINTER_setPrinterName(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString name;
   if (get_qstring(p, name, xsink))
      return 0;
   qp->setPrinterName(name);
   return 0;
}

//void setPrinterSelectionOption ( const QString & option )
static QoreNode *QPRINTER_setPrinterSelectionOption(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   QString option;
   if (get_qstring(p, option, xsink))
      return 0;
   qp->setPrinterSelectionOption(option);
   return 0;
}

//void setResolution ( int dpi )
static QoreNode *QPRINTER_setResolution(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int dpi = p ? p->getAsInt() : 0;
   qp->setResolution(dpi);
   return 0;
}

#ifdef WINDOWS
//void setWinPageSize ( int pageSize )
static QoreNode *QPRINTER_setWinPageSize(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QoreNode *p = get_param(params, 0);
   int pageSize = p ? p->getAsInt() : 0;
   qp->setWinPageSize(pageSize);
   return 0;
}

//QList<PaperSource> supportedPaperSources () const
static QoreNode *QPRINTER_supportedPaperSources(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<QPrinter::PaperSource> qlist = qp->supportedPaperSources();

   QoreListNode *l = new QoreListNode();
   for (QList<QPrinter::PaperSource>::iterator i = qlist.begin(), e = qlist.end(); i != e; ++i)
      l->push(new QoreBigIntNode(*i));

   return l;
}
#endif

//QList<int> supportedResolutions () const
static QoreNode *QPRINTER_supportedResolutions(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   QList<int> ilist_rv = qp->supportedResolutions();
   QoreListNode *l = new QoreListNode();
   for (QList<int>::iterator i = ilist_rv.begin(), e = ilist_rv.end(); i != e; ++i)
      l->push(new QoreBigIntNode((*i)));
   return l;
}

//int toPage () const
static QoreNode *QPRINTER_toPage(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->toPage());
}

#ifdef WINDOWS
//int winPageSize () const
static QoreNode *QPRINTER_winPageSize(QoreObject *self, QoreQPrinter *qp, const QoreListNode *params, ExceptionSink *xsink)
{
   return new QoreBigIntNode(qp->winPageSize());
}
#endif

QoreClass *initQPrinterClass(QoreClass *qpaintdevice)
{
   QC_QPrinter = new QoreClass("QPrinter", QDOM_GUI);
   CID_QPRINTER = QC_QPrinter->getID();

   QC_QPrinter->addBuiltinVirtualBaseClass(qpaintdevice);

   QC_QPrinter->setConstructor(QPRINTER_constructor);
   QC_QPrinter->setCopy((q_copy_t)QPRINTER_copy);

   QC_QPrinter->addMethod("abort",                       (q_method_t)QPRINTER_abort);
   QC_QPrinter->addMethod("collateCopies",               (q_method_t)QPRINTER_collateCopies);
   QC_QPrinter->addMethod("colorMode",                   (q_method_t)QPRINTER_colorMode);
   QC_QPrinter->addMethod("creator",                     (q_method_t)QPRINTER_creator);
   QC_QPrinter->addMethod("docName",                     (q_method_t)QPRINTER_docName);
   QC_QPrinter->addMethod("doubleSidedPrinting",         (q_method_t)QPRINTER_doubleSidedPrinting);
   QC_QPrinter->addMethod("fontEmbeddingEnabled",        (q_method_t)QPRINTER_fontEmbeddingEnabled);
   QC_QPrinter->addMethod("fromPage",                    (q_method_t)QPRINTER_fromPage);
   QC_QPrinter->addMethod("fullPage",                    (q_method_t)QPRINTER_fullPage);
   QC_QPrinter->addMethod("newPage",                     (q_method_t)QPRINTER_newPage);
   QC_QPrinter->addMethod("numCopies",                   (q_method_t)QPRINTER_numCopies);
   QC_QPrinter->addMethod("orientation",                 (q_method_t)QPRINTER_orientation);
   QC_QPrinter->addMethod("outputFileName",              (q_method_t)QPRINTER_outputFileName);
   QC_QPrinter->addMethod("outputFormat",                (q_method_t)QPRINTER_outputFormat);
   QC_QPrinter->addMethod("pageOrder",                   (q_method_t)QPRINTER_pageOrder);
   QC_QPrinter->addMethod("pageRect",                    (q_method_t)QPRINTER_pageRect);
   QC_QPrinter->addMethod("pageSize",                    (q_method_t)QPRINTER_pageSize);
   //QC_QPrinter->addMethod("paintEngine",                 (q_method_t)QPRINTER_paintEngine);
   QC_QPrinter->addMethod("paperRect",                   (q_method_t)QPRINTER_paperRect);
   QC_QPrinter->addMethod("paperSource",                 (q_method_t)QPRINTER_paperSource);
   //QC_QPrinter->addMethod("printEngine",                 (q_method_t)QPRINTER_printEngine);
   QC_QPrinter->addMethod("printProgram",                (q_method_t)QPRINTER_printProgram);
   QC_QPrinter->addMethod("printRange",                  (q_method_t)QPRINTER_printRange);
   QC_QPrinter->addMethod("printerName",                 (q_method_t)QPRINTER_printerName);
   QC_QPrinter->addMethod("printerSelectionOption",      (q_method_t)QPRINTER_printerSelectionOption);
   QC_QPrinter->addMethod("printerState",                (q_method_t)QPRINTER_printerState);
   QC_QPrinter->addMethod("resolution",                  (q_method_t)QPRINTER_resolution);
   QC_QPrinter->addMethod("setCollateCopies",            (q_method_t)QPRINTER_setCollateCopies);
   QC_QPrinter->addMethod("setColorMode",                (q_method_t)QPRINTER_setColorMode);
   QC_QPrinter->addMethod("setCreator",                  (q_method_t)QPRINTER_setCreator);
   QC_QPrinter->addMethod("setDocName",                  (q_method_t)QPRINTER_setDocName);
   QC_QPrinter->addMethod("setDoubleSidedPrinting",      (q_method_t)QPRINTER_setDoubleSidedPrinting);
   QC_QPrinter->addMethod("setFontEmbeddingEnabled",     (q_method_t)QPRINTER_setFontEmbeddingEnabled);
   QC_QPrinter->addMethod("setFromTo",                   (q_method_t)QPRINTER_setFromTo);
   QC_QPrinter->addMethod("setFullPage",                 (q_method_t)QPRINTER_setFullPage);
   QC_QPrinter->addMethod("setNumCopies",                (q_method_t)QPRINTER_setNumCopies);
   QC_QPrinter->addMethod("setOrientation",              (q_method_t)QPRINTER_setOrientation);
   QC_QPrinter->addMethod("setOutputFileName",           (q_method_t)QPRINTER_setOutputFileName);
   QC_QPrinter->addMethod("setOutputFormat",             (q_method_t)QPRINTER_setOutputFormat);
   QC_QPrinter->addMethod("setPageOrder",                (q_method_t)QPRINTER_setPageOrder);
   QC_QPrinter->addMethod("setPageSize",                 (q_method_t)QPRINTER_setPageSize);
   QC_QPrinter->addMethod("setPaperSource",              (q_method_t)QPRINTER_setPaperSource);
   QC_QPrinter->addMethod("setPrintProgram",             (q_method_t)QPRINTER_setPrintProgram);
   QC_QPrinter->addMethod("setPrintRange",               (q_method_t)QPRINTER_setPrintRange);
   QC_QPrinter->addMethod("setPrinterName",              (q_method_t)QPRINTER_setPrinterName);
   QC_QPrinter->addMethod("setPrinterSelectionOption",   (q_method_t)QPRINTER_setPrinterSelectionOption);
   QC_QPrinter->addMethod("setResolution",               (q_method_t)QPRINTER_setResolution);
#ifdef WINDOWS
   QC_QPrinter->addMethod("setWinPageSize",              (q_method_t)QPRINTER_setWinPageSize);
   QC_QPrinter->addMethod("supportedPaperSources",       (q_method_t)QPRINTER_supportedPaperSources);
#endif
   QC_QPrinter->addMethod("supportedResolutions",        (q_method_t)QPRINTER_supportedResolutions);
   QC_QPrinter->addMethod("toPage",                      (q_method_t)QPRINTER_toPage);
#ifdef WINDOWS
   QC_QPrinter->addMethod("winPageSize",                 (q_method_t)QPRINTER_winPageSize);
#endif

   return QC_QPrinter;
}
