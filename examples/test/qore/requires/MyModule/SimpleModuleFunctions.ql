# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file SimpleModuleFunctions.ql consists functions definitions

%new-style
%enable-all-warnings
%require-types
%strict-args

public namespace EXAMPLE_F;

public string sub EXAMPLE_F::func(string str) {
  return str + "EXAMPLE_F::func\n";
}

