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

#ifndef OHOS_DAUDIO_IPC_CALLBACK_PROXY_H
#define OHOS_DAUDIO_IPC_CALLBACK_PROXY_H

#include "idaudio_ipc_callback.h"
#include "iremote_broker.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace DistributedHardware {
class DAudioIpcCallbackProxy : public IRemoteProxy<IDAudioIpcCallback> {
public:
    explicit DAudioIpcCallbackProxy(const sptr<IRemoteObject> impl) : IRemoteProxy<IDAudioIpcCallback>(impl) {}

    ~DAudioIpcCallbackProxy() {}
    int32_t OnNotifyRegResult(const std::string &devId, const std::string &dhId, const std::string &reqId,
        int32_t status, const std::string &resultData) override;
    int32_t OnNotifyUnregResult(const std::string &devId, const std::string &dhId, const std::string &reqId,
        int32_t status, const std::string &resultData) override;

private:
    static inline BrokerDelegator<DAudioIpcCallbackProxy> delegator_;
};
} // DistributedHardware
} // OHOS
#endif // OHOS_DAUDIO_IPC_CALLBACK_PROXY_H
