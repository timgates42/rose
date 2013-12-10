
#include "DLX/Core/constructs.hpp"
#include "DLX/Core/frontend.hpp"
#include "DLX/OpenACC/language.hpp"

#include <cassert>

class SgLocatedNode;

namespace DLX {

namespace Directives {

template <>
generic_construct_t<OpenACC::language_t> * buildConstruct<OpenACC::language_t>(OpenACC::language_t::construct_kinds_e kind) {
  switch (kind) {
    case OpenACC::language_t::e_acc_construct_data:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_data>();
    case OpenACC::language_t::e_acc_construct_parallel:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel>();
    case OpenACC::language_t::e_acc_construct_kernel:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_kernel>();
    case OpenACC::language_t::e_acc_construct_loop:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop>();
    case OpenACC::language_t::e_acc_construct_host_data:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_host_data>();
    case OpenACC::language_t::e_acc_construct_declare:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_declare>();
    case OpenACC::language_t::e_acc_construct_cache:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_cache>();
    case OpenACC::language_t::e_acc_construct_update:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_update>();
    case OpenACC::language_t::e_acc_construct_blank:
     return new construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_blank>();
    case OpenACC::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

template <>
bool findAssociatedNodes<OpenACC::language_t>(SgLocatedNode * directive_node, generic_construct_t<OpenACC::language_t> * construct) {
  switch (construct->kind) {
    case OpenACC::language_t::e_acc_construct_data:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_data>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_data> *)construct
     );
    case OpenACC::language_t::e_acc_construct_parallel:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_parallel>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> *)construct
     );
    case OpenACC::language_t::e_acc_construct_kernel:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_kernel>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_kernel> *)construct
     );
    case OpenACC::language_t::e_acc_construct_loop:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_loop>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> *)construct
     );
    case OpenACC::language_t::e_acc_construct_host_data:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_host_data>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_host_data> *)construct
     );
    case OpenACC::language_t::e_acc_construct_declare:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_declare>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_declare> *)construct
     );
    case OpenACC::language_t::e_acc_construct_cache:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_cache>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_cache> *)construct
     );
    case OpenACC::language_t::e_acc_construct_update:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_update>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_update> *)construct
     );
    case OpenACC::language_t::e_acc_construct_blank:
     return Frontend::Frontend<OpenACC::language_t>::findAssociatedNodes<OpenACC::language_t::e_acc_construct_blank>(
       directive_node, (construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_blank> *)construct
     );
    case OpenACC::language_t::e_construct_last:
     assert(false);
    default:
     assert(false);
  }
}

}

}

