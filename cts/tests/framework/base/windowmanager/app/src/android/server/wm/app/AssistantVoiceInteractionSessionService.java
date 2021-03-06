/*
 * Copyright (C) 2017 The Android Open Source Project
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
 * limitations under the License
 */

package android.server.wm.app;

import android.content.Intent;
import android.os.Bundle;
import android.service.voice.VoiceInteractionSession;
import android.service.voice.VoiceInteractionSessionService;

public class AssistantVoiceInteractionSessionService extends VoiceInteractionSessionService {

    @Override
    public VoiceInteractionSession onNewSession(Bundle args) {
        return new VoiceInteractionSession(this) {
            @Override
            public void onPrepareShow(Bundle args, int showFlags) {
                setUiEnabled(false);
            }

            @Override
            public void onShow(Bundle args, int showFlags) {
                Intent i = new Intent(AssistantVoiceInteractionSessionService.this,
                        AssistantActivity.class);
                if (args != null) {
                    i.putExtras(args);
                }
                startAssistantActivity(i);
            }
        };
    }
}
