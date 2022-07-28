/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 */

#include "sinkhandlersubscribelocalhardware_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "daudio_sink_handler.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace DistributedHardware {
void SinkHandlerSubscribeLocalHardwareFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < 0))  {
        return;
    }
    std::string param(reinterpret_cast<const char*>(data), size);
    std::string dhId(reinterpret_cast<const char*>(data), size);

    DAudioSinkHandler::GetInstance().SubscribeLocalHardware(dhId, param);
}
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DistributedHardware::SinkHandlerSubscribeLocalHardwareFuzzTest(data, size);
    return 0;
}

