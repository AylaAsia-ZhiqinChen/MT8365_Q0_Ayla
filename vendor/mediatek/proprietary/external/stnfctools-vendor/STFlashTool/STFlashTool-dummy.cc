
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/system_properties.h>

#include "nfa_api.h"
#include "NfcAdaptation.h"

static bool sIsNfaEnabled = false;
static pthread_mutex_t mNfaMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t mNfaCond = PTHREAD_COND_INITIALIZER;

static void nfaDeviceManagementCallback(uint8_t dmEvent,
                                        tNFA_DM_CBACK_DATA* eventData) {
  printf("%s: event=0x%X", __FUNCTION__, dmEvent);
  switch (dmEvent) {
    case NFA_DM_ENABLE_EVT: { /* Result of NFA_Enable */
      (void)pthread_mutex_lock(&mNfaMutex);
      sIsNfaEnabled = eventData->enable.status == NFA_STATUS_OK;
      (void)pthread_cond_signal(&mNfaCond);
      (void)pthread_mutex_unlock(&mNfaMutex);
    } break;
    case NFA_DM_DISABLE_EVT: { /* Result of NFA_Disable */
      (void)pthread_mutex_lock(&mNfaMutex);
      sIsNfaEnabled = false;
      (void)pthread_cond_signal(&mNfaCond);
      (void)pthread_mutex_unlock(&mNfaMutex);
    } break;
    case NFA_DM_NFCC_TRANSPORT_ERR_EVT: { /* Result of NFA_Enable */
      (void)pthread_mutex_lock(&mNfaMutex);
      (void)pthread_cond_signal(&mNfaCond);
      (void)pthread_mutex_unlock(&mNfaMutex);
    } break;
    default:
      printf("%s: event=0x%X not handled", __FUNCTION__, dmEvent);
  }
}

static void nfaConnectionCallback(uint8_t connEvent,
                                  tNFA_CONN_EVT_DATA* eventData) {
  (void)eventData;
  printf("%s: event= %u", __FUNCTION__, connEvent);
}

char* prop_name = NULL;

int parse_cmd_line(int argc, char* argv[]) {
  int c;
  while ((c = getopt(argc, argv, "vlfic:CP:d:erwos:t:")) != -1) {
    switch (c) {
      case 'v':
        // Verbose option to display debug information
        printf("Verbosity ignored\n");
        break;
      case 'l':
        /* Request to flash new Loader instead of NFC firmware */
        printf(
            "Option not supported: -l. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'f':
        /* Force update, regardless of FW version on chip */
        printf(
            "Option not supported: -f. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'i':
        /* only info requested, disable update */
        printf(
            "Option not supported: -i. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'c':
        /* fw configuration option : applies configuration from file */
        printf(
            "'-c %s' option IGNORED, the config file is read from "
            "libnfc-hal-st.conf\n",
            optarg);
        break;
      case 'C':
        // Verifies that the chip has a firmware and a configuration applied.
        printf(
            "Option not supported: -C. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'P':
        // Verifies that the chip has a firmware and a configuration applied.
        prop_name = optarg;
        break;
      case 'd':
        /* Force device name instead using default name */
        printf(
            "Option not supported: -d. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'e':
        /* Erase loader memory */
        printf(
            "Option not supported: -e. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'r':
        /* recovery mode */
        printf(
            "Option not supported: -r. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 'w':
        /* aWake mode */
        printf("Option not implemented yet: -w. Highlight if you need it.\n");
        return -1;
      case 'o':
        /* don t go back to Quickboot mode */
        printf("-o option IGNORED, this is always the behavior\n");
        break;
      case 's':
        /* script option : applies scenario from script file */
        printf(
            "Option not supported: -s. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case 't':
        /* Test option */
        printf(
            "Option not supported: -t. Revert to legacy STFlashTool for "
            "this.\n");
        return -1;
      case '?':
        printf("Error on option `-%c'.\n", optopt);
        return -1;
    }
  }
  if (optind < argc) {
    printf("Option not recognized: %s\n", argv[optind]);
    return -1;
  }
  return 0;
}

int main(int argc, char* argv[]) {
  tHAL_NFC_ENTRY* halFuncEntries;
  tNFA_STATUS stat = NFA_STATUS_OK;

  printf("Dummy STFlashTool version, rely on HAL firmware update\n");

  if (parse_cmd_line(argc, argv) < 0) {
    printf("ERROR, not opening HAL\n");
    return -1;
  }

  NfcAdaptation& theInstance = NfcAdaptation::GetInstance();
  theInstance.Initialize();  // start GKI, NCI task, NFC task

  halFuncEntries = theInstance.GetHalEntryFuncs();
  NFA_Init(halFuncEntries);

  (void)pthread_mutex_lock(&mNfaMutex);
  stat = NFA_Enable(nfaDeviceManagementCallback, nfaConnectionCallback);
  if (stat == NFA_STATUS_OK) {
    struct timespec ts;
    (void)clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 20;  // in case loader + fw update
    (void)pthread_cond_timedwait(&mNfaCond, &mNfaMutex, &ts);
  }

  if (prop_name != NULL) {
    // Set the property
    int ret = __system_property_set(
        prop_name, sIsNfaEnabled != true ? "failed" : "success");
    if (ret != 0) {
      printf("ERROR: cannot set system property '%s'\n", prop_name);
    }
  }

  if (sIsNfaEnabled == true) {
    stat = NFA_Disable(TRUE /* graceful */);
    struct timespec ts;
    (void)clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 3;
    (void)pthread_cond_timedwait(&mNfaCond, &mNfaMutex, &ts);
  } else {
    stat = NFA_STATUS_FAILED;
  }
  (void)pthread_mutex_unlock(&mNfaMutex);

  theInstance.Finalize();

  if (stat == NFA_STATUS_OK) {
    printf("Operation completed successfully\n");
  } else {
    printf("Operation completed with errors\n");
  }

  return 0;
}