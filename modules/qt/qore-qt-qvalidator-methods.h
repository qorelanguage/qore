
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

	 QoreNode *val = new QoreStringNode(input.toUtf8().data(), QCS_UTF8);
	 ExceptionSink xsink;
	 LVarInstantiatorHelper lvh("arg0", val, &xsink);

	 ReferenceHolder<QoreList> args(new QoreList(), &xsink);
	 args->push(lvh.getArg());

	 // execute method and discard any return value
	 discard(m_fixup->eval(qore_obj, *args, &xsink), &xsink);
	 
	 QoreNode *str = lvh.getOutputValue();
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
	 LVarInstantiatorHelper arg0("arg0", new QoreStringNode(input.toUtf8().data(), QCS_UTF8), &xsink);
	 LVarInstantiatorHelper arg1("arg1", new QoreNode((int64)pos), &xsink);

	 ReferenceHolder<QoreList> args(new QoreList(), &xsink);
	 args->push(arg0.getArg());
	 args->push(arg1.getArg());

	 // execute method and discard any return value
	 QoreNode *rv = m_fixup->eval(qore_obj, *args, &xsink);
	 QValidator::State state = (QValidator::State)(rv ? rv->getAsInt() : 0);
	 discard(rv, &xsink);
	 
	 // rewrite results to args
	 if (!xsink) {
	    get_qstring(arg0.getOutputValue(), input, &xsink);
	    QoreNode *n_pos = arg1.getOutputValue();
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
