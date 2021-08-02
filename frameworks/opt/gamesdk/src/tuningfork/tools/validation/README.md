# Tuning Fork Validation tool

This tool validates Tuning Fork proto and settings files in an APK.

## tuningfork_settings

The APK must contain *assets/tuningfork/tuningfork_settings.bin* file with
serialized data for `Settings` proto message:

```proto
message Settings {
  message Histogram {
    optional int32 instrument_key = 1;
    optional float bucket_min = 2;
    optional float bucket_max = 3;
    optional int32 n_buckets = 4;
  }
  message AggregationStrategy {
    enum Submission {
      TIME_BASED = 1;
      TICK_BASED = 2;
    }
    optional Submission method = 1;
    optional int32 intervalms_or_count = 2;
    optional int32 max_instrumentation_keys = 3;
    repeated int32 annotation_enum_size = 4;
  }
  optional AggregationStrategy aggregation_strategy = 1;
  repeated Histogram histograms = 2;
}
```

### Settings validation

* At least one histogram
* `max_instrumentation_keys` must be between 1 and 256
* `annotation_enum_size` must match `Annotation` message (see below)

### Example
Example of data before serialization:

```textproto
aggregation_strategy:
{
  method: TIME_BASED,
  intervalms_or_count: 600000,
  max_instrumentation_keys: 2,
  annotation_enum_size: [3, 4]
}
histograms:
[
  {
    instrument_key: 0,
    bucket_min: 28,
    bucket_max: 32,
    n_buckets: 70
  },
  {
    instrument_key: 1,
    bucket_min: 28,
    bucket_max: 32,
    n_buckets: 70
  }
]
```

## dev_tuningfork.proto

Apk must contain *assets/tuningfork/dev_tuningfork.proto* file with `Annotation`
and `FidelityParams` proto message.

### Validation

Both messages (`Annotation` and `FidelityParams`) must follow these rules
* No oneofs
* No Nested types
* No extensions

Additional limitation for `Annotation` message only
* Only `ENUM` types
* Size of enums must match 'annotation_enum_size` field in settings.

Additional limitation for `FidelityParams` messsage only
* Only `ENUM`, `FLOAT` and `INT32` types

### Example

Valid .proto file:

```proto
syntax = "proto3";

package com.google.tuningfork;

enum LoadingState {
  UNKNOWN = 0;
  LOADING = 1;
  NOT_LOADING = 2;
}

enum Level {
  UNKNOWN = 0;
  Level_1 = 1;
  Level_2 = 2;
  Level_3 = 3;
}

message Annotation {
  LoadingState loading_state = 1;
  Level level = 2;
}

enum QualitySettings {
  UNKNOWN = 0;
  FASTEST = 1;
  FAST = 2;
  SIMPLE = 3;
  GOOD = 4;
  BEAUTIFUL = 5;
  FANTASTIC = 6;
}

message FidelityParams {
  QualitySettings quality_settings = 1;
  int32 lod_level = 2;
  float distance = 3;
}
```

### Annotation size explanation

*annotation_enum_size* from Settings proto must match 'Annotation' message

From `Annotation` message example above:
  * number of enum fields for `LoadingState` enum is 3
  * number of enum fields for `Level` enum is 4
  * `Annotation` message contains two fields - 'loading_state' and 'level'
  * `annotation_enum_size` must be [3, 4]

## dev_tuningfork_fidelityparams

The APK must contain at least one file in assets/tuningfork folder with pattern
*dev_tuningfork_fidelityparams_.{1,15}.bin*. Each file contains serialized
parameters for `FidelityParams` proto message from *dev_tuningfork.proto* file.


