#ifndef QORE_TUXEDO_MODULE_H_
#define QORE_TUXEDO_MODULE_H_

class QoreString* tuxedo_module_init();
void tuxedo_module_ns_init(class Namespace* rns, class Namespace* qns);
void tuxedo_module_delete();


#endif

// EOF

