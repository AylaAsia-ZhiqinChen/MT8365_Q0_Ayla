# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: vti/dashboard/proto/TestSuiteResultMessage.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from vti.test_serving.proto import TestScheduleConfigMessage_pb2 as vti_dot_test__serving_dot_proto_dot_TestScheduleConfigMessage__pb2


DESCRIPTOR = _descriptor.FileDescriptor(
  name='vti/dashboard/proto/TestSuiteResultMessage.proto',
  package='android.vts',
  syntax='proto2',
  serialized_pb=_b('\n0vti/dashboard/proto/TestSuiteResultMessage.proto\x12\x0b\x61ndroid.vts\x1a\x36vti/test_serving/proto/TestScheduleConfigMessage.proto\"\x92\x05\n\x16TestSuiteResultMessage\x12\x14\n\x0c\x61\x63\x63\x65ss_token\x18\x01 \x01(\t\x12\x13\n\x0bresult_path\x18\x02 \x01(\t\x12\x1a\n\x0c\x62oot_success\x18\x03 \x01(\x08:\x04true\x12\x0e\n\x06\x62ranch\x18\x0b \x02(\t\x12\x0e\n\x06target\x18\x0c \x02(\t\x12\x10\n\x08\x62uild_id\x18\r \x02(\t\x12\x12\n\nsuite_plan\x18\x15 \x01(\t\x12\x15\n\rsuite_version\x18\x16 \x01(\t\x12\x1a\n\x12suite_build_number\x18\x17 \x01(\t\x12\x12\n\nstart_time\x18\x18 \x01(\x03\x12\x10\n\x08\x65nd_time\x18\x19 \x01(\x03\x12\x11\n\thost_name\x18\x1a \x01(\t\x12\x12\n\nsuite_name\x18\x1b \x01(\t\x12 \n\x18\x62uild_system_fingerprint\x18\x1f \x01(\t\x12 \n\x18\x62uild_vendor_fingerprint\x18  \x01(\t\x12\x14\n\ttest_type\x18! \x01(\x05:\x01\x30\x12\x1e\n\x16passed_test_case_count\x18) \x01(\x05\x12\x1e\n\x16\x66\x61iled_test_case_count\x18* \x01(\x05\x12\x14\n\x0cmodules_done\x18+ \x01(\x05\x12\x15\n\rmodules_total\x18, \x01(\x05\x12\x16\n\x0einfra_log_path\x18\x33 \x01(\t\x12\x1b\n\x13repacked_image_path\x18\x34 \x03(\t\x12\x17\n\x0fvendor_build_id\x18= \x01(\t\x12\x14\n\x0cgsi_build_id\x18> \x01(\t\x12@\n\x0fschedule_config\x18? \x01(\x0b\x32\'.android.test.lab.ScheduleConfigMessageB6\n\x15\x63om.android.vts.protoB\x1bTestSuiteResultMessageProtoP\x00')
  ,
  dependencies=[vti_dot_test__serving_dot_proto_dot_TestScheduleConfigMessage__pb2.DESCRIPTOR,])
_sym_db.RegisterFileDescriptor(DESCRIPTOR)




_TESTSUITERESULTMESSAGE = _descriptor.Descriptor(
  name='TestSuiteResultMessage',
  full_name='android.vts.TestSuiteResultMessage',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='access_token', full_name='android.vts.TestSuiteResultMessage.access_token', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='result_path', full_name='android.vts.TestSuiteResultMessage.result_path', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='boot_success', full_name='android.vts.TestSuiteResultMessage.boot_success', index=2,
      number=3, type=8, cpp_type=7, label=1,
      has_default_value=True, default_value=True,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='branch', full_name='android.vts.TestSuiteResultMessage.branch', index=3,
      number=11, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='target', full_name='android.vts.TestSuiteResultMessage.target', index=4,
      number=12, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='build_id', full_name='android.vts.TestSuiteResultMessage.build_id', index=5,
      number=13, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='suite_plan', full_name='android.vts.TestSuiteResultMessage.suite_plan', index=6,
      number=21, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='suite_version', full_name='android.vts.TestSuiteResultMessage.suite_version', index=7,
      number=22, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='suite_build_number', full_name='android.vts.TestSuiteResultMessage.suite_build_number', index=8,
      number=23, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='start_time', full_name='android.vts.TestSuiteResultMessage.start_time', index=9,
      number=24, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='end_time', full_name='android.vts.TestSuiteResultMessage.end_time', index=10,
      number=25, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='host_name', full_name='android.vts.TestSuiteResultMessage.host_name', index=11,
      number=26, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='suite_name', full_name='android.vts.TestSuiteResultMessage.suite_name', index=12,
      number=27, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='build_system_fingerprint', full_name='android.vts.TestSuiteResultMessage.build_system_fingerprint', index=13,
      number=31, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='build_vendor_fingerprint', full_name='android.vts.TestSuiteResultMessage.build_vendor_fingerprint', index=14,
      number=32, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='test_type', full_name='android.vts.TestSuiteResultMessage.test_type', index=15,
      number=33, type=5, cpp_type=1, label=1,
      has_default_value=True, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='passed_test_case_count', full_name='android.vts.TestSuiteResultMessage.passed_test_case_count', index=16,
      number=41, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='failed_test_case_count', full_name='android.vts.TestSuiteResultMessage.failed_test_case_count', index=17,
      number=42, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='modules_done', full_name='android.vts.TestSuiteResultMessage.modules_done', index=18,
      number=43, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='modules_total', full_name='android.vts.TestSuiteResultMessage.modules_total', index=19,
      number=44, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='infra_log_path', full_name='android.vts.TestSuiteResultMessage.infra_log_path', index=20,
      number=51, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='repacked_image_path', full_name='android.vts.TestSuiteResultMessage.repacked_image_path', index=21,
      number=52, type=9, cpp_type=9, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='vendor_build_id', full_name='android.vts.TestSuiteResultMessage.vendor_build_id', index=22,
      number=61, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='gsi_build_id', full_name='android.vts.TestSuiteResultMessage.gsi_build_id', index=23,
      number=62, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='schedule_config', full_name='android.vts.TestSuiteResultMessage.schedule_config', index=24,
      number=63, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto2',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=122,
  serialized_end=780,
)

_TESTSUITERESULTMESSAGE.fields_by_name['schedule_config'].message_type = vti_dot_test__serving_dot_proto_dot_TestScheduleConfigMessage__pb2._SCHEDULECONFIGMESSAGE
DESCRIPTOR.message_types_by_name['TestSuiteResultMessage'] = _TESTSUITERESULTMESSAGE

TestSuiteResultMessage = _reflection.GeneratedProtocolMessageType('TestSuiteResultMessage', (_message.Message,), dict(
  DESCRIPTOR = _TESTSUITERESULTMESSAGE,
  __module__ = 'vti.dashboard.proto.TestSuiteResultMessage_pb2'
  # @@protoc_insertion_point(class_scope:android.vts.TestSuiteResultMessage)
  ))
_sym_db.RegisterMessage(TestSuiteResultMessage)


DESCRIPTOR.has_options = True
DESCRIPTOR._options = _descriptor._ParseOptions(descriptor_pb2.FileOptions(), _b('\n\025com.android.vts.protoB\033TestSuiteResultMessageProtoP\000'))
# @@protoc_insertion_point(module_scope)
