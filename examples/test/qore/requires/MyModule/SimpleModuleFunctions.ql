# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file SimpleModuleFunctions.ql is the main include file for all simple modules

%new-style

# uses hard typing; safe with '%require-types'
%requires qore >= 0.8.6

# uses xml functionality
# %requires xml

public namespace EXAMPLE_F;

#! dome function
/** @deprecated try to create the better one
  */
public deprecated string sub EXAMPLE_F::func(string str) {
  return str + "EXAMPLE_F::func\n";
}
