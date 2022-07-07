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

#ifndef OHOS_SOFTBUS_ADAPTER_TEST_UTILS_H
#define OHOS_SOFTBUS_ADAPTER_TEST_UTILS_H

#include "isoftbus_listener.h"

namespace OHOS {
namespace DistributedHardware {
class MockISoftbusListener : public ISoftbusListener {
public:
    MockISoftbusListener() {}
    ~MockISoftbusListener() {}

    void OnSessionOpened(int32_t sessionId, int32_t result) {}
    void OnSessionClosed(int32_t sessionId) {}
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen) {}
    void OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *streamFrameInfo) {}
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_SOFTBUS_ADAPTER_TEST_UTILS_H
