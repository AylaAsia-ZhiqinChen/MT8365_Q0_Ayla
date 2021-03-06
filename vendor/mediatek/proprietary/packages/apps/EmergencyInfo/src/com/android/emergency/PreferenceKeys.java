/*
 * Copyright (C) 2016 The Android Open Source Project
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
package com.android.emergency;

/**
 * Contains the keys of the preferences used in this app.
 */
public interface PreferenceKeys {

    /** Key for the medical info preference category in the edit screen. */
    public static final String KEY_MEDICAL_INFO = "medical_info";

    /** Key for the medical info preference */
    public static final String KEY_EDIT_MEDICAL_INFO = "edit_medical_info";

    /** Key for emergency contacts preference */
    public static final String KEY_EMERGENCY_CONTACTS = "emergency_contacts";

    /// M: SIM contact support @{
    public static final String KEY_CONTACT_SUFFIX_MAP = "_map";
    public static final String KEY_CONTACT_SUFFIX_NUM = "_num";
    /// M: @}

    /** Key for the add emergency contact preference */
    public static final String KEY_ADD_EMERGENCY_CONTACT = "add_emergency_contact";

    /** Key for emergency name preference */
    public static final String KEY_NAME = "name";

    /** Key to store and read the address of the user. */
    public static final String KEY_ADDRESS = "address";

    /** Key to store and read the blood type of the user. */
    public static final String KEY_BLOOD_TYPE = "blood_type";

    /** Key to store and read the allergies of the user. */
    public static final String KEY_ALLERGIES = "allergies";

    /** Key to store and read the medications of the user. */
    public static final String KEY_MEDICATIONS = "medications";

    /** Key to store and read the medical conditions of the user. */
    public static final String KEY_MEDICAL_CONDITIONS = "medical_conditions";

    /** Key to store and read the organ donor choice of the user. */
    public static final String KEY_ORGAN_DONOR = "organ_donor";

    /**
     * Keys for all editable emergency info preferences.
     *
     * <p>Note: Do not change the order of these keys, since the order is used to collect TRON stats
     */
    public static final String[] KEYS_EDIT_EMERGENCY_INFO = {KEY_ADDRESS,
            KEY_BLOOD_TYPE, KEY_ALLERGIES, KEY_MEDICATIONS,
            KEY_MEDICAL_CONDITIONS, KEY_ORGAN_DONOR};

    /** Keys for all viewable emergency info preferences */
    public static final String[] KEYS_VIEW_EMERGENCY_INFO = {KEY_ADDRESS, KEY_BLOOD_TYPE,
            KEY_ALLERGIES, KEY_MEDICATIONS, KEY_MEDICAL_CONDITIONS, KEY_ORGAN_DONOR};
}
