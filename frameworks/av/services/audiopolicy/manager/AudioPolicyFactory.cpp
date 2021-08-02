/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <AudioPolicyManager.h>

namespace android {
#if defined(MTK_AUDIO)
AudioPolicyManagerCustomInterface* audiopolicymanagerMTK = NULL;  // MTK_AUDIO
#endif
extern "C" AudioPolicyInterface* createAudioPolicyManager(
        AudioPolicyClientInterface *clientInterface)
{
#if defined(MTK_AUDIO)
    audiopolicymanagerMTK = (AudioPolicyManagerCustomInterface*) new AudioPolicyManagerCustomImpl();
#endif
    return new AudioPolicyManager(clientInterface, audiopolicymanagerMTK);
}

extern "C" void destroyAudioPolicyManager(AudioPolicyInterface *interface)
{
#if defined(MTK_AUDIO)
    if (audiopolicymanagerMTK != NULL) {    // MTK_AUDIO
        delete audiopolicymanagerMTK;
    }
#endif
    delete interface;
}

} // namespace android
