/*
 * Copyright (C) 2019 The Android Open Source Project
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

package android.platform.helpers;

public interface IAutoMapsHelper extends IAppHelper {
    /**
     * Setup expectations: Maps app is open
     *
     * This method is used to search an address.
     *
     */
    void search(String address);

    /**
     * Setup expectations: Maps app is open
     *
     * This method is used to start navigation.
     *
     */
    void startNavigation();

    /**
     * Setup expectations: Maps app is open
     *
     * This method is used to stop navigation.
     *
     */
    void stopNavigation();
}