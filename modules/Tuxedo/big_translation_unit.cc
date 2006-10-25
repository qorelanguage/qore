// Helper used to compile all translation units as one
// to make the build faster.

#ifdef USE_ONE_TRANSLATION_UNIT

#define SKIP_THIS_FILE

#include "fml_api.cc"
#include "low_level_api.cc"
#include "QC_TuxedoTypedBuffer.cc"
#include "QoreTuxedoTypedBuffer.cc"
#include "hashed_parameters_helper.cc"
#include "QC_TuxedoContext.cc"
#include "QC_TuxedoQueueControlParams.cc"
#include "QC_TuxedoTransactionId.cc"
#include "tuxedo.cc"

#endif

// EOF


