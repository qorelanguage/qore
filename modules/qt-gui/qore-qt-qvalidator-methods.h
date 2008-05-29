
#if 0
class T {
#endif

   public:
      DLLLOCAL virtual void fixup ( QString & input ) const
      {
	 if (!m_fixup) {
	    QOREQTYPE::fixup(input);
	    return;
	 }

	 AbstractQoreNode *val = new QoreStringNode(input.toUtf8().data(), QCS_UTF8);
	 ExceptionSink xsink;
	 ReferenceArgumentHelper lvh(val, &xsink);

	 ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(lvh.getArg());

	 // execute method and discard any return value
	 discard(qore_obj->evalMethod(*m_fixup, *args, &xsink), &xsink);
	 
	 AbstractQoreNode *str = lvh.getOutputValue();
	 get_qstring(str, input, &xsink);
      }
      DLLLOCAL virtual void fixup_parent ( QString & input ) const
      {
	 QOREQTYPE::fixup(input);
      }
      DLLLOCAL virtual State validate ( QString & input, int & pos ) const
      {
	 if (!m_validate)
	    return validate_parent(input, pos);

	 ExceptionSink xsink;
	 ReferenceArgumentHelper arg0(new QoreStringNode(input.toUtf8().data(), QCS_UTF8), &xsink);
	 ReferenceArgumentHelper arg1(new QoreBigIntNode(pos), &xsink);

	 ReferenceHolder<QoreListNode> args(new QoreListNode(), &xsink);
	 args->push(arg0.getArg());
	 args->push(arg1.getArg());

	 // execute method and discard any return value
	 ReferenceHolder<AbstractQoreNode> rv(qore_obj->evalMethod(*m_fixup, *args, &xsink), &xsink);
	 if (xsink)
	    return (QValidator::State)0;

	 QValidator::State state = (QValidator::State)(rv ? rv->getAsInt() : 0);
	 
	 // rewrite results to args
	 if (!xsink) {
	    get_qstring(arg0.getOutputValue(), input, &xsink);
	    AbstractQoreNode *n_pos = arg1.getOutputValue();
	    pos = n_pos ? n_pos->getAsInt() : 0;
	 }
	 return state;
      }
#ifdef _IS_QORE_QVALIDATOR
      DLLLOCAL virtual State validate_parent ( QString & input, int & pos ) const
      {
	 return QValidator::Invalid;
      }
#else
      DLLLOCAL virtual State validate_parent ( QString & input, int & pos ) const
      {
	 return QOREQTYPE::validate(input, pos);
      }
#endif
      
#if 0
}
#endif
