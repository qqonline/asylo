/*
 *
 * Copyright 2018 Asylo authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "asylo/platform/arch/include/trusted/register_signal.h"

#include <signal.h>

#include "asylo/util/logging.h"
#include "asylo/platform/arch/include/trusted/host_calls.h"
#include "asylo/platform/common/bridge_types.h"
#include "asylo/platform/posix/signal/signal_manager.h"

namespace asylo {

// Translates |bridge_signum| to the value inside the enclave, and passes it to
// the signal handler registered inside enclave.
void TranslateAndHandleSignal(int bridge_signum) {
  int signum = FromBridgeSignal(bridge_signum);
  if (signum < 0) {
    return;
  }
  sighandler_t handler = SignalManager::GetInstance()->GetSignalHandler(signum);
  if (!handler) {
    return;
  }
  handler(signum);
}

}  // namespace asylo

extern "C" int enc_register_signal(int signum, const char *enclave_name,
                                   size_t len) {
  struct sigaction act;
  act.sa_handler = &asylo::TranslateAndHandleSignal;
  return enc_untrusted_register_signal_handler(signum, &act, enclave_name, len);
}
