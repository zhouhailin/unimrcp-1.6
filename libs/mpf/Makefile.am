MAINTAINERCLEANFILES     = Makefile.in

AM_CPPFLAGS              = -I$(top_srcdir)/libs/mpf/codecs \
                           -I$(top_srcdir)/libs/mpf/include \
                           -I$(top_srcdir)/libs/mpf \
                           -I$(top_srcdir)/libs/apr-toolkit/include \
                           $(UNIMRCP_APR_INCLUDES)

noinst_LTLIBRARIES       = libmpf.la

#libmpf_la_LDFLAGS       = -L$(top_srcdir)/libs/mpf/ -lcommonaudio
#libmpf_ladir            = $(libdir)
#libmpf_la_DATA          = $(top_srcdir)/libs/mpf/libcommonaudio.so

include_HEADERS          = codecs/g711/g711.h \
                           include/mpf.h \
                           include/mpf_activity_detector.h \
                           include/mpf_audio_file_descriptor.h \
                           include/mpf_audio_file_stream.h \
                           include/mpf_bridge.h \
                           include/mpf_buffer.h \
                           include/mpf_codec.h \
                           include/mpf_codec_descriptor.h \
                           include/mpf_codec_manager.h \
                           include/mpf_context.h \
                           include/mpf_dtmf_detector.h \
                           include/mpf_dtmf_generator.h \
                           include/mpf_engine.h \
                           include/mpf_engine_factory.h \
                           include/mpf_frame.h \
                           include/mpf_frame_buffer.h \
                           include/mpf_message.h \
                           include/mpf_mixer.h \
                           include/mpf_multiplier.h \
                           include/mpf_named_event.h \
                           include/mpf_object.h \
                           include/mpf_stream.h \
                           include/mpf_stream_descriptor.h \
                           include/mpf_termination.h \
                           include/mpf_termination_factory.h \
                           include/mpf_rtp_termination_factory.h \
                           include/mpf_file_termination_factory.h \
                           include/mpf_scheduler.h \
                           include/mpf_types.h \
                           include/mpf_encoder.h \
                           include/mpf_decoder.h \
                           include/mpf_jitter_buffer.h \
                           include/mpf_rtp_header.h \
                           include/mpf_rtp_descriptor.h \
                           include/mpf_rtp_stream.h \
                           include/mpf_rtp_stat.h \
                           include/mpf_rtp_defs.h \
                           include/mpf_rtp_attribs.h \
                           include/mpf_rtp_pt.h \
                           include/mpf_rtcp_packet.h \
                           include/mpf_resampler.h \
                           webrtc/typedefs.h \
                           webrtc/system_wrappers/interface/cpu_features_wrapper.h \
                           webrtc/common_audio/signal_processing/include/real_fft.h \
                           webrtc/common_audio/signal_processing/include/signal_processing_library.h \
                           webrtc/common_audio/signal_processing/include/spl_inl.h \
                           webrtc/common_audio/vad/include/webrtc_vad.h

libmpf_la_SOURCES        = codecs/g711/g711.c \
                           src/mpf_activity_detector.c \
                           src/mpf_audio_file_stream.c \
                           src/mpf_bridge.c \
                           src/mpf_buffer.c \
                           src/mpf_codec_descriptor.c \
                           src/mpf_codec_g711.c \
                           src/mpf_codec_linear.c \
                           src/mpf_codec_manager.c \
                           src/mpf_context.c \
                           src/mpf_dtmf_detector.c \
                           src/mpf_dtmf_generator.c \
                           src/mpf_engine.c \
                           src/mpf_engine_factory.c \
                           src/mpf_mixer.c \
                           src/mpf_multiplier.c \
                           src/mpf_named_event.c \
                           src/mpf_termination.c \
                           src/mpf_termination_factory.c \
                           src/mpf_rtp_termination_factory.c \
                           src/mpf_file_termination_factory.c \
                           src/mpf_frame_buffer.c \
                           src/mpf_scheduler.c \
                           src/mpf_encoder.c \
                           src/mpf_decoder.c \
                           src/mpf_jitter_buffer.c \
                           src/mpf_rtp_stream.c \
                           src/mpf_rtp_attribs.c \
                           src/mpf_resampler.c \
                           src/mpf_stream.c \
                           webrtc/common_audio/signal_processing/complex_bit_reverse.c \
                           webrtc/common_audio/signal_processing/complex_fft_tables.h \
                           webrtc/common_audio/signal_processing/complex_fft.c \
                           webrtc/common_audio/signal_processing/cross_correlation.c \
                           webrtc/common_audio/signal_processing/division_operations.c \
                           webrtc/common_audio/signal_processing/downsample_fast.c \
                           webrtc/common_audio/signal_processing/energy.c \
                           webrtc/common_audio/signal_processing/get_scaling_square.c \
                           webrtc/common_audio/signal_processing/min_max_operations.c \
                           webrtc/common_audio/signal_processing/real_fft.c \
                           webrtc/common_audio/signal_processing/resample_48khz.c \
                           webrtc/common_audio/signal_processing/resample_by_2_internal.h \
                           webrtc/common_audio/signal_processing/resample_by_2_internal.c \
                           webrtc/common_audio/signal_processing/resample_fractional.c \
                           webrtc/common_audio/signal_processing/spl_init.c \
                           webrtc/common_audio/signal_processing/vector_scaling_operations.c \
                           webrtc/common_audio/vad/vad_core.h \
                           webrtc/common_audio/vad/vad_filterbank.h \
                           webrtc/common_audio/vad/vad_gmm.h \
                           webrtc/common_audio/vad/vad_sp.h \
                           webrtc/common_audio/vad/vad_core.c \
                           webrtc/common_audio/vad/vad_filterbank.c \
                           webrtc/common_audio/vad/vad_gmm.c \
                           webrtc/common_audio/vad/vad_sp.c \
                           webrtc/common_audio/vad/webrtc_vad.c 
