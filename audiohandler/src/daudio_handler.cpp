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

#include "daudio_handler.h"

#include <vector>

#include "audio_system_manager.h"
#include "avcodec_list.h"
#include "nlohmann/json.hpp"
#include "string_ex.h"


#include "daudio_constants.h"
#include "daudio_errorcode.h"
#include "daudio_log.h"
#include "daudio_util.h"

using json = nlohmann::json;

namespace OHOS {
namespace DistributedHardware {
IMPLEMENT_SINGLE_INSTANCE(DAudioHandler);

DAudioHandler::DAudioHandler()
{
    DHLOGI("%s: DAudioHandler constructed.", LOG_TAG);
}

DAudioHandler::~DAudioHandler()
{
    DHLOGI("%s: DAudioHandler destructed.", LOG_TAG);
}

int32_t DAudioHandler::Initialize()
{
    DHLOGI("%s: Initialize.", LOG_TAG);
    int32_t ret = QueryCodecInfo();
    if (ret != DH_SUCCESS) {
        DHLOGI("%s: QueryCodecInfo failed.", LOG_TAG);
        return ret;
    }
    ret = QueryAudioInfo();
    if (ret != DH_SUCCESS) {
        DHLOGI("%s: QueryAudioInfo failed.", LOG_TAG);
        return ret;
    }
    GetSupportAudioInfo(audioInfos_, encoderInfos_, decoderInfos_);
    return DH_SUCCESS;
}

std::vector<DHItem> DAudioHandler::Query()
{
    DHLOGI("%s: Query hardware info.", LOG_TAG);
    auto audioSrv = AudioStandard::AudioSystemManager::GetInstance();
    std::vector<DHItem> dhItemVec;
    if (audioSrv == nullptr) {
        DHLOGE("%s: Can not get audio system manager.", LOG_TAG);
        return dhItemVec;
    }

    auto audioDevices = audioSrv->GetDevices(AudioStandard::DeviceFlag::ALL_DEVICES_FLAG);
    for (auto dev : audioDevices) {
        int32_t dhId;
        if (dev->deviceType_ == AudioStandard::DeviceType::DEVICE_TYPE_SPEAKER) {
            dhId = PIN_OUT_SPEAKER;
        } else if (dev->deviceType_ == AudioStandard::DeviceType::DEVICE_TYPE_MIC) {
            dhId = PIN_IN_MIC;
        } else {
            dhId = PIN_OUT_DAUDIO_DEFAULT;
        }

        json infoJson;
        int32_t deviceType = GetDevTypeByDHId(dhId);
        if (deviceType == AUDIO_DEVICE_TYPE_MIC) {
            infoJson["SampleRates"] = micInfos_.sampleRates;
            infoJson["ChannelMasks"] = micInfos_.channels;
            infoJson["Formats"] = micInfos_.formats;
        } else if (deviceType == AUDIO_DEVICE_TYPE_SPEAKER) {
            infoJson["SampleRates"] = spkInfos_.sampleRates;
            infoJson["ChannelMasks"] = spkInfos_.channels;
            infoJson["Formats"] = spkInfos_.formats;
        }
        DHItem dhItem;
        dhItem.dhId = std::to_string(dhId);
        dhItem.attrs = infoJson.dump();
        dhItemVec.push_back(dhItem);
        DHLOGD("%s: Query result: dhId: %d, attrs: %s.", LOG_TAG, GetAnonyString(dhId).c_str(),
            infoJson.dump().c_str());
    }
    return dhItemVec;
}

int32_t DAudioHandler::QueryCodecInfo()
{
    DHLOGI("%s: QueryCodecInfo", LOG_TAG);
    auto avCodecList = Media::AVCodecListFactory::CreateAVCodecList();
    if (avCodecList == nullptr) {
        DHLOGE("%s: Query avcodec info failed.", LOG_TAG);
        return ERR_DH_AUDIO_NULLPTR;
    }

    bool queryFlag = false;
    for (auto codec : avCodecList->GetAudioEncoderCaps()) {
        if (codec == nullptr || codec->GetCodecInfo() == nullptr || codec->GetCodecInfo()->GetName() != AVENC_AAC) {
            continue;
        }
        encoderInfos_.sampleRates = codec->GetSupportedSampleRates();
        encoderInfos_.formats = codec->GetSupportedFormats();
        encoderInfos_.channelMaxVal = codec->GetSupportedChannel().maxVal;
        encoderInfos_.channelMinVal = codec->GetSupportedChannel().minVal;
        queryFlag = true;
    }

    for (auto codec : avCodecList->GetAudioDecoderCaps()) {
        if (codec == nullptr || codec->GetCodecInfo() == nullptr || codec->GetCodecInfo()->GetName() != AVENC_AAC) {
            continue;
        }
        decoderInfos_.sampleRates = codec->GetSupportedSampleRates();
        decoderInfos_.formats = codec->GetSupportedFormats();
        decoderInfos_.channelMaxVal = codec->GetSupportedChannel().maxVal;
        decoderInfos_.channelMinVal = codec->GetSupportedChannel().minVal;
        queryFlag = true;
    }

    if (!queryFlag) {
        DHLOGE("%s: QueryCodecInfo failed.", LOG_TAG);
        return ERR_DH_AUDIO_FAILED;
    }
    return DH_SUCCESS;
}

int32_t DAudioHandler::QueryAudioInfo()
{
    DHLOGE("%s: begin QueryCodecInfo.", LOG_TAG);
    audioInfos_.sampleRates = OHOS::AudioStandard::AudioCapturer::GetSupportedSamplingRates();
    audioInfos_.formats = OHOS::AudioStandard::AudioCapturer::GetSupportedFormats();
    audioInfos_.channels = OHOS::AudioStandard::AudioCapturer::GetSupportedChannels();

    return DH_SUCCESS;
}

void DAudioHandler::GetSupportAudioInfo(AudioInfo &audioInfos, CoderInfo &encoderInfos,
    CoderInfo &decoderInfos)
{
    for (auto iter = audioInfos.sampleRates.begin(); iter != audioInfos.sampleRates.end(); iter++) {
        if (std::find(encoderInfos.sampleRates.begin(), encoderInfos.sampleRates.end(), *iter) !=
            encoderInfos.sampleRates.end()) {
            micInfos_.sampleRates.push_back(*iter);
        }
        if (std::find(decoderInfos.sampleRates.begin(), decoderInfos.sampleRates.end(), *iter) !=
            decoderInfos.sampleRates.end()) {
            spkInfos_.sampleRates.push_back(*iter);
        }
    }

    for (auto iter = audioInfos.formats.begin(); iter != audioInfos.formats.end(); iter++) {
        if (std::find(encoderInfos.formats.begin(), encoderInfos.formats.end(), *iter) != encoderInfos.formats.end()) {
            micInfos_.formats.push_back(*iter);
        }
        if (std::find(decoderInfos.formats.begin(), decoderInfos.formats.end(), *iter) != decoderInfos.formats.end()) {
            spkInfos_.formats.push_back(*iter);
        }
    }

    for (auto iter = audioInfos.channels.begin(); iter != audioInfos.channels.end(); iter++) {
        if (*iter <= encoderInfos.channelMaxVal && *iter >= encoderInfos.channelMinVal) {
            micInfos_.channels.push_back(*iter);
        }
        if (*iter <= decoderInfos.channelMaxVal && *iter >= decoderInfos.channelMinVal) {
            spkInfos_.channels.push_back(*iter);
        }
    }
    if (micInfos_.sampleRates.empty()) {
        micInfos_.sampleRates.push_back(SAMPLE_RATE_DEFAULT);
    }
    if (spkInfos_.sampleRates.empty()) {
        spkInfos_.sampleRates.push_back(SAMPLE_RATE_DEFAULT);
    }
    if (micInfos_.channels.empty()) {
        micInfos_.channels.push_back(CHANNEL_COUNT_DEFAULT);
    }
    if (spkInfos_.channels.empty()) {
        spkInfos_.channels.push_back(CHANNEL_COUNT_DEFAULT);
    }
    if (micInfos_.formats.empty()) {
        micInfos_.channels.push_back(SAMPLE_FORMAT_DEFAULT);
    }
    if (spkInfos_.formats.empty()) {
        spkInfos_.channels.push_back(SAMPLE_FORMAT_DEFAULT);
    }
}

std::map<std::string, std::string> DAudioHandler::QueryExtraInfo()
{
    DHLOGI("%s: QueryExtraInfo", LOG_TAG);
    std::map<std::string, std::string> extraInfo;
    return extraInfo;
}

bool DAudioHandler::IsSupportPlugin()
{
    DHLOGI("%s: IsSupportPlugin", LOG_TAG);
    return false;
}

void DAudioHandler::RegisterPluginListener(std::shared_ptr<PluginListener> listener)
{
    DHLOGI("%s: Register plugin listener", LOG_TAG);
    listener_ = listener;
}

void DAudioHandler::UnRegisterPluginListener()
{
    DHLOGI("%s: UnRegister plugin listener", LOG_TAG);
    listener_ = nullptr;
}

IHardwareHandler* GetHardwareHandler()
{
    return &DAudioHandler::GetInstance();
}
} // namespace DistributedHardware
} // namespace OHOS