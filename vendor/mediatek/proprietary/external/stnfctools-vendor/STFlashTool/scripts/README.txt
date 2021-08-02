This folder contains the RF parameters and NFC chipset configuration related to the platform.
The Android FileSystem will use a copy of the following file during boot of Factory Mode and Normal Mode:
* if image is configured for 2UICC: script_DB10mtk_2UICC.txt
* if image is configured for 1UICC or 1UICC+1eSE: script_DB10mtk_UICC_eSE.txt
* if image is configured for 2UICC+1eSE: script_DB10mtk_2UICC_eSE.txt

For a new platform, please contact your ST Microelectronics FAE for help with preparing this file properly.
Your FAE will typically generate a file "st21nfc_conf.txt", please rename accordingly to the platform configuration.
