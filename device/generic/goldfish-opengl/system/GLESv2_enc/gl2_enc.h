// Generated Code - DO NOT EDIT !!
// generated by 'emugen'

#ifndef GUARD_gl2_encoder_context_t
#define GUARD_gl2_encoder_context_t

#include "IOStream.h"
#include "ChecksumCalculator.h"
#include "gl2_client_context.h"


#include <string.h>
#include "glUtils.h"
#include "GL2EncoderUtils.h"

struct gl2_encoder_context_t : public gl2_client_context_t {

	IOStream *m_stream;
	ChecksumCalculator *m_checksumCalculator;

	gl2_encoder_context_t(IOStream *stream, ChecksumCalculator *checksumCalculator);
	virtual uint64_t lockAndWriteDma(void* data, uint32_t sz) { return 0; }
};

#endif  // GUARD_gl2_encoder_context_t
