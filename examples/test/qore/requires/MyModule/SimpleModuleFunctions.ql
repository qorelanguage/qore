# -*- mode: qore; indent-tabs-mode: nil -*-
#! @file SimpleModuleFunctions.ql consists functions definitions

%new-style

public namespace EXAMPLE_F;

public string sub EXAMPLE_F::func(string str) {
  return str + "EXAMPLE_F::func\n";
}

