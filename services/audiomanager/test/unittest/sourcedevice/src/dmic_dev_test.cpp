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

#include "dmic_dev_test.h"

#include "audio_decode_transport.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
constexpr int32_t DH_ID = 1;
constexpr int32_t DH_ID_MIC = 134217728;
const std::string DEV_ID = "Test_Dev_Id";
const std::string CAP = "Test_Capability";

void DMicDevTest::SetUpTestCase(void) {}

void DMicDevTest::TearDownTestCase(void) {}

void DMicDevTest::SetUp(void)
{
    eventCb_ = std::make_shared<MockIAudioEventCallback>();
    mic_ = std::make_shared<DMicDev>(DEV_ID, eventCb_);
}

void DMicDevTest::TearDown(void)
{
    eventCb_ = nullptr;
    mic_ = nullptr;
}

/**
 * @tc.name: EnableDMic_001
 * @tc.desc: Verify EnableDMic and EnableDevice function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, EnableDMic_001, TestSize.Level1)
{
    EXPECT_EQ(ERR_DH_AUDIO_FAILED, mic_->EnableDMic(DH_ID, CAP));
    EXPECT_EQ(ERR_DH_AUDIO_HDI_PROXY_NOT_INIT, mic_->EnableDevice(DH_ID, CAP));

    mic_->enabledPorts_.insert(DH_ID_MIC);
    EXPECT_EQ(ERR_DH_AUDIO_HDI_PROXY_NOT_INIT, mic_->EnableDMic(DH_ID_MIC, CAP));
}

/**
 * @tc.name: DisableDMic_001
 * @tc.desc: Verify DisableDMic and DisableDevice function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, DisableDMic_001, TestSize.Level1)
{
    mic_->enabledPorts_.insert(DH_ID);
    EXPECT_EQ(ERR_DH_AUDIO_HDI_PROXY_NOT_INIT, mic_->DisableDevice(DH_ID));

    EXPECT_EQ(ERR_DH_AUDIO_FAILED, mic_->DisableDMic(DH_ID));

    mic_->curPort_ = DH_ID_MIC;
    EXPECT_EQ(ERR_DH_AUDIO_FAILED, mic_->DisableDMic(DH_ID_MIC));
    EXPECT_FALSE(mic_->IsOpened());
}

/**
 * @tc.name: OpenDevice_001
 * @tc.desc: Verify OpenDevice function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, OpenDevice_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, mic_->OpenDevice(DEV_ID, DH_ID));

    eventCb_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_SA_MICCALLBACK_NULL, mic_->OpenDevice(DEV_ID, DH_ID));
}

/**
 * @tc.name: CloseDevice_001
 * @tc.desc: Verify CloseDevice function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, CloseDevice_001, TestSize.Level1)
{
    EXPECT_EQ(DH_SUCCESS, mic_->CloseDevice(DEV_ID, DH_ID));

    eventCb_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_SA_MICCALLBACK_NULL, mic_->CloseDevice(DEV_ID, DH_ID));
}

/**
 * @tc.name: SetParameters_001
 * @tc.desc: Verify SetParameters and GetAudioParam function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, SetParameters_001, TestSize.Level1)
{
    const AudioParamHDF param = {
        .sampleRate = SAMPLE_RATE_8000,
        .channelMask = STEREO,
        .bitFormat = SAMPLE_U8,
        .streamUsage = STREAM_USAGE_UNKNOWN,
        .frameSize = 30,
        .period = 0,
        .ext = "Test",
    };
    EXPECT_EQ(DH_SUCCESS, mic_->SetParameters(DEV_ID, DH_ID, param));
    mic_->GetAudioParam();
}

/**
 * @tc.name: NotifyEvent_001
 * @tc.desc: Verify NotifyEvent function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, NotifyEvent_001, TestSize.Level1)
{
    AudioEvent event = AudioEvent(OPEN_MIC, "OPEN_MIC");
    EXPECT_EQ(DH_SUCCESS, mic_->NotifyEvent(DEV_ID, DH_ID, event));

    event.type = EVENT_UNKNOWN;
    EXPECT_EQ(DH_SUCCESS, mic_->NotifyEvent(DEV_ID, DH_ID, event));

    eventCb_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_SA_EVENT_CALLBACK_NULL, mic_->NotifyEvent(DEV_ID, DH_ID, event));
}

/**
 * @tc.name: SetUp_001
 * @tc.desc: Verify SetUp function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, SetUp_001, TestSize.Level1)
{
    mic_->micTrans_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_TRANS_ERROR, mic_->SetUp());

    mic_->micTrans_ = std::make_shared<MockIAudioDataTransport>();
    EXPECT_EQ(DH_SUCCESS, mic_->SetUp());
}

/**
 * @tc.name: Start_001
 * @tc.desc: Verify Start and IsOpened function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, Start_001, TestSize.Level1)
{
    mic_->micTrans_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_SA_MIC_TRANS_NULL, mic_->Start());

    mic_->micTrans_ = std::make_shared<AudioDecodeTransport>(DEV_ID);
    EXPECT_NE(DH_SUCCESS, mic_->Start());
    EXPECT_FALSE(mic_->IsOpened());

    mic_->micTrans_ = std::make_shared<MockIAudioDataTransport>();
    mic_->isTransReady_.store(true);
    EXPECT_EQ(DH_SUCCESS, mic_->Start());
    EXPECT_TRUE(mic_->IsOpened());
}

/**
 * @tc.name: Start_002
 * @tc.desc: Verify Start and IsOpened function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, Start_002, TestSize.Level1)
{
    mic_->micTrans_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_TRANS_ERROR, mic_->SetUp());
    EXPECT_NE(DH_SUCCESS, mic_->Start());

    mic_->micTrans_ = std::make_shared<MockIAudioDataTransport>();
    EXPECT_EQ(DH_SUCCESS, mic_->SetUp());
    EXPECT_EQ(ERR_DH_AUDIO_SA_MIC_CHANNEL_WAIT_TIMEOUT, mic_->Start());
    EXPECT_FALSE(mic_->IsOpened());

    mic_->isTransReady_.store(true);
    EXPECT_EQ(DH_SUCCESS, mic_->Start());
    EXPECT_TRUE(mic_->IsOpened());
}

/**
 * @tc.name: Stop_001
 * @tc.desc: Verify Stop and IsOpened function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, Stop_001, TestSize.Level1)
{
    mic_->micTrans_ = nullptr;
    EXPECT_EQ(DH_SUCCESS, mic_->Stop());

    mic_->micTrans_ = std::make_shared<AudioDecodeTransport>(DEV_ID);
    EXPECT_EQ(DH_SUCCESS, mic_->Stop());

    mic_->micTrans_ = std::make_shared<MockIAudioDataTransport>();
    EXPECT_EQ(DH_SUCCESS, mic_->Stop());
    EXPECT_FALSE(mic_->IsOpened());
}

/**
 * @tc.name: Stop_002
 * @tc.desc: Verify Stop and IsOpened function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, Stop_002, TestSize.Level1)
{
    mic_->micTrans_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_TRANS_ERROR, mic_->SetUp());
    EXPECT_NE(DH_SUCCESS, mic_->Start());
    EXPECT_EQ(DH_SUCCESS, mic_->Stop());
    EXPECT_FALSE(mic_->IsOpened());
}

/**
 * @tc.name: Release_001
 * @tc.desc: Verify Release function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, Release_001, TestSize.Level1)
{
    mic_->micTrans_ = nullptr;
    EXPECT_EQ(DH_SUCCESS, mic_->Release());

    mic_->micTrans_ = std::make_shared<AudioDecodeTransport>(DEV_ID);
    EXPECT_EQ(DH_SUCCESS, mic_->Release());

    mic_->micTrans_ = std::make_shared<MockIAudioDataTransport>();
    EXPECT_EQ(DH_SUCCESS, mic_->Release());
}


/**
 * @tc.name: ReadStreamData_001
 * @tc.desc: Verify ReadStreamData and WriteStreamData function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, ReadStreamData_001, TestSize.Level1)
{
    mic_->curStatus_ = AudioStatus::STATUS_START;
    const size_t capacity = 1;
    auto writeData = std::make_shared<AudioData>(capacity);
    EXPECT_EQ(DH_SUCCESS, mic_->WriteStreamData(DEV_ID, DH_ID, writeData));

    std::shared_ptr<AudioData> readData = nullptr;
    mic_->dataQueue_.push(writeData);
    EXPECT_EQ(DH_SUCCESS, mic_->ReadStreamData(DEV_ID, DH_ID, readData));

    std::shared_ptr<AudioData> readData1 = nullptr;
    EXPECT_EQ(DH_SUCCESS, mic_->ReadStreamData(DEV_ID, DH_ID, readData1));
}

/**
 * @tc.name: NotifyHdfAudioEvent_001
 * @tc.desc: Verify NotifyHdfAudioEvent function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, NotifyHdfAudioEvent_001, TestSize.Level1)
{
    AudioEvent event = AudioEvent(OPEN_MIC, "OPEN_MIC");
    EXPECT_EQ(DH_SUCCESS, mic_->NotifyHdfAudioEvent(event));

    event.type = MIC_OPENED;
    mic_->curPort_ = DH_ID_MIC;
    EXPECT_EQ(DH_SUCCESS, mic_->NotifyHdfAudioEvent(event));
}

/**
 * @tc.name: OnStateChange_001
 * @tc.desc: Verify OnStateChange function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, OnStateChange_001, TestSize.Level1)
{
    AudioEventType event = DATA_OPENED;
    EXPECT_EQ(DH_SUCCESS, mic_->OnStateChange(event));

    event = DATA_CLOSED;
    EXPECT_EQ(DH_SUCCESS, mic_->OnStateChange(event));

    event = EVENT_UNKNOWN;
    EXPECT_EQ(DH_SUCCESS, mic_->OnStateChange(event));

    eventCb_ = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_SA_MICCALLBACK_NULL, mic_->OnStateChange(event));
}

/**
 * @tc.name: OnDecodeTransDataDone_001
 * @tc.desc: Verify OnDecodeTransDataDone function.
 * @tc.type: FUNC
 * @tc.require: AR000H0E5F
 */
HWTEST_F(DMicDevTest, OnDecodeTransDataDone_001, TestSize.Level1)
{
    std::shared_ptr<AudioData> data = nullptr;
    EXPECT_EQ(ERR_DH_AUDIO_NULLPTR, mic_->OnDecodeTransDataDone(data));

    const size_t capacity = 1;
    data = std::make_shared<AudioData>(capacity);
    for (size_t i = 1; i <= mic_->DATA_QUEUE_MAX_SIZE + 1; i++) {
        EXPECT_EQ(DH_SUCCESS, mic_->OnDecodeTransDataDone(data));
    }
}
} // namespace DistributedHardware
} // namespace OHOS
