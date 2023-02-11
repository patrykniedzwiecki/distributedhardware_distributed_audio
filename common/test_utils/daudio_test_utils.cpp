/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "daudio_test_utils.h"

#include <iostream>
#include <string>

#include "aec_test.h"
#include "daudio_errorcode.h"
#include "local_audio.h"

using namespace std;

const string CMD_QUIT = "quit";
const string CMD_QUIT_EXT = "0";
const string CMD_LOCAL_CAPTURE = "lcap";
const string CMD_LOCAL_CAPTURE_EXT = "20";
const string CMD_LOCAL_RENDER = "lren";
const string CMD_LOCAL_RENDER_EXT = "21";
const string CMD_AEC_TEST = "aec";
const string CMD_AEC_TEST_EXT = "22";
namespace OHOS {
namespace DistributedHardware {

void DAudioTestUtils::DoAudioTest()
{
    bool running = true;
    string cmd = "";
    while (running) {
        cout << endl <<"Input test command: ";
        cin >> cmd;
        cout <<endl;

        if (cmd == CMD_QUIT || cmd == CMD_QUIT_EXT) {
            running = false;
            continue;
        }

        if (cmd == CMD_LOCAL_CAPTURE || cmd == CMD_LOCAL_CAPTURE_EXT) {
            LocalCapture();
            continue;
        }

        if (cmd == CMD_LOCAL_RENDER || cmd == CMD_LOCAL_RENDER_EXT) {
            LocalRender();
            continue;
        }

        if (cmd == CMD_AEC_TEST || cmd == CMD_AEC_TEST_EXT) {
            LocalEchoCancel();
            continue;
        }
    }
}

void DAudioTestUtils::LocalCapture()
{
    cout << "[LocalCapture]" << endl;
    AudioCaptureObj capture;
    AudioBufferInfo info;
    int32_t res = capture.ReadAudioInfo(info);
    if (res != DH_SUCCESS) {
        return;
    }
    capture.Init(info);
    int32_t time = 0;
    cout << "Input capture time(s): ";
    cin >> time;
    cout << endl;
    capture.CaptureFrame(time);
    capture.Release();
}

void DAudioTestUtils::LocalRender()
{
    cout << "[LocalRender]" << endl;
    AudioRenderObj render;
    string path;
    AudioBufferInfo info;

    cout << "Input file path: ";
    cin >> path;
    cout << endl;

    size_t pos = path.find(".wav");
    if (pos != string::npos) {
        if (render.ReadWavFile(path, info) != DH_SUCCESS) {
            return;
        }
    }
    pos = path.find(".pcm");
    if (pos != string::npos) {
        if (render.ReadPcmFile(path, info) != DH_SUCCESS) {
            return;
        }
    }

    render.Init(info);
    render.RenderFrame();
    render.Release();
}

void DAudioTestUtils::LocalEchoCancel()
{
    cout << "[LocalEchoCancel]" << endl;
    AecTest echoCancel;
    echoCancel.Init();
    echoCancel.Process();
    echoCancel.Release();
}
} // namespace DistributedHardware
} // namespace OHOS

int32_t main()
{
    cout << "**********************************************************************************" << endl
         << "Distributed Audio Test Demo Bin." << endl
         << "**********************************************************************************" << endl;
    OHOS::DistributedHardware::DAudioTestUtils testBin;
    testBin.DoAudioTest();
    return 0;
}