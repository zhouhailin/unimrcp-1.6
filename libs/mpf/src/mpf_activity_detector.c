/*
 * Copyright 2008-2015 Arsen Chaloyan
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mpf_activity_detector.h"
#include "apt_log.h"
#include "webrtc/common_audio/vad/include/webrtc_vad.h"

/** Detector states */
typedef enum {
	DETECTOR_STATE_INACTIVITY,           /**< inactivity detected */
	DETECTOR_STATE_ACTIVITY_TRANSITION,  /**< activity detection is in-progress */
	DETECTOR_STATE_ACTIVITY,             /**< activity detected */
	DETECTOR_STATE_INACTIVITY_TRANSITION /**< inactivity detection is in-progress */
} mpf_detector_state_e;

/** Activity detector */
struct mpf_activity_detector_t {
	/* voice activity (silence) level threshold */
	apr_size_t           level_threshold;

	/* period of activity required to complete transition to active state */
	apr_size_t           speech_timeout;
	/* period of inactivity required to complete transition to inactive state */
	apr_size_t           silence_timeout;
	/* noinput timeout */
	apr_size_t           noinput_timeout;

	/* current state */
	mpf_detector_state_e state;
	/* duration spent in current state  */
	apr_size_t           duration;
};

/** Create activity detector */
MPF_DECLARE(mpf_activity_detector_t*) mpf_activity_detector_create(apr_pool_t *pool)
{
	mpf_activity_detector_t *detector = apr_palloc(pool,sizeof(mpf_activity_detector_t));
	detector->level_threshold = 30; /* 0 .. 255 */
	detector->speech_timeout = 200; /* 0.2 s */
	detector->silence_timeout = 200; /* 0.2 s */
	detector->noinput_timeout = 5000; /* 5 s */
	detector->duration = 0;
	detector->state = DETECTOR_STATE_INACTIVITY;
	return detector;
}

/** Reset activity detector */
MPF_DECLARE(void) mpf_activity_detector_reset(mpf_activity_detector_t *detector)
{
	detector->duration = 0;
	detector->state = DETECTOR_STATE_INACTIVITY;
}

/** Set threshold of voice activity (silence) level */
MPF_DECLARE(void) mpf_activity_detector_level_set(mpf_activity_detector_t *detector, apr_size_t level_threshold)
{
	detector->level_threshold = level_threshold;
}

/** Set noinput timeout */
MPF_DECLARE(void) mpf_activity_detector_noinput_timeout_set(mpf_activity_detector_t *detector, apr_size_t noinput_timeout)
{
	detector->noinput_timeout = noinput_timeout;
}

/** Set timeout required to trigger speech (transition from inactive to active state) */
MPF_DECLARE(void) mpf_activity_detector_speech_timeout_set(mpf_activity_detector_t *detector, apr_size_t speech_timeout)
{
	detector->speech_timeout = speech_timeout;
}

/** Set timeout required to trigger silence (transition from active to inactive state) */
MPF_DECLARE(void) mpf_activity_detector_silence_timeout_set(mpf_activity_detector_t *detector, apr_size_t silence_timeout)
{
	detector->silence_timeout = silence_timeout;
}


static APR_INLINE void mpf_activity_detector_state_change(mpf_activity_detector_t *detector, mpf_detector_state_e state)
{
	detector->duration = 0;
	detector->state = state;
	apt_log(MPF_LOG_MARK,APT_PRIO_INFO,"Activity Detector state changed [%d]",state);
}

/*static apr_size_t mpf_activity_detector_level_calculate(const mpf_frame_t *frame)
{
	apr_size_t sum = 0;
	apr_size_t count = frame->codec_frame.size/2;
	const apr_int16_t *cur = frame->codec_frame.buffer;
	const apr_int16_t *end = cur + count;

	for(; cur < end; cur++) {
		if(*cur < 0) {
			sum -= *cur;
		}
		else {
			sum += *cur;
		}
	}

	return sum / count;
}*/

static apr_size_t mpf_activity_detector_level_calculate(const mpf_frame_t *frame)
{
  apr_size_t samplesCount = frame->codec_frame.size/2;
  int per_ms_frames = 20;
  apr_size_t sampleRate = 8000;
  size_t samples = sampleRate * per_ms_frames / 1000;
  if (samples == 0) return -1;
  size_t nTotal = (samplesCount / samples);
  int16_t *input = frame->codec_frame.buffer;
  VadInst *vadInst;
  if (WebRtcVad_Create(&vadInst)) {
    return -1;
  }
  int status = WebRtcVad_Init(vadInst);
  if (status != 0) {
    WebRtcVad_Free(vadInst);
    return -1;
  }
  int16_t vad_mode = 1;
  status = WebRtcVad_set_mode(vadInst, vad_mode);
  if (status != 0) {
    WebRtcVad_Free(vadInst);
    return -1;
  }
  int cnt = 0;
  int i  = 0;
  if(nTotal > 0) {
    for (i = 0; i < nTotal; i++) {
    //int keep_weight = 0;
    int nVadRet = WebRtcVad_Process(vadInst, sampleRate, input, samples);
   // printf("==========%d=============== \n", nVadRet);
    if (nVadRet == -1) {
      WebRtcVad_Free(vadInst);
      return -1;
    } else {
      if (nVadRet >= 1) {
        cnt++;
      } 
   }
    input += samples;
   }
   WebRtcVad_Free(vadInst);
  if (cnt < nTotal/10) {
    return 0;
  } else {
    return 1;
  }
 }
 
 if(nTotal == 0) {
   int nVadRet = WebRtcVad_Process(vadInst, sampleRate, input, samplesCount);
   //printf("==========%d=============== \n", nVadRet);
   WebRtcVad_Free(vadInst);
   return nVadRet;
 }

 return 0;
}

MPF_DECLARE(mpf_detector_event_e) mpf_activity_detector_process(mpf_activity_detector_t *detector, const mpf_frame_t *frame)
{
    mpf_detector_event_e det_event = MPF_DETECTOR_EVENT_NONE;
    apr_size_t level = 0;
    if((frame->type & MEDIA_FRAME_TYPE_AUDIO) == MEDIA_FRAME_TYPE_AUDIO) {     
        level = mpf_activity_detector_level_calculate(frame);
#if 0
        apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Activity Detector --------------------- [%"APR_SIZE_T_FMT"]",level);
#endif
    }

    if(detector->state == DETECTOR_STATE_INACTIVITY) {
        if(level >= 1) {
   //         apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Activity Detector ----DETECTOR_STATE_ACTIVITY_TRANSITION---------------- [%"APR_SIZE_T_FMT"]",level);
            mpf_activity_detector_state_change(detector, DETECTOR_STATE_ACTIVITY_TRANSITION);
        }
        else {
            detector->duration += CODEC_FRAME_TIME_BASE;
            if(detector->duration >= detector->noinput_timeout) {
                det_event = MPF_DETECTOR_EVENT_NOINPUT;
            }
        }
    }
    else if(detector->state == DETECTOR_STATE_ACTIVITY_TRANSITION) {
        if(level >= 1) {
            detector->duration += CODEC_FRAME_TIME_BASE;
     //       apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Activity Detector ----DETECTOR_STATE_ACTIVITY-------11111--------- [%"APR_SIZE_T_FMT"]",level);
            if(detector->duration >= detector->speech_timeout) {
                det_event = MPF_DETECTOR_EVENT_ACTIVITY;
                mpf_activity_detector_state_change(detector, DETECTOR_STATE_ACTIVITY);
            }
        }
        else {
            mpf_activity_detector_state_change(detector,DETECTOR_STATE_INACTIVITY);
        }
    }
    else if(detector->state == DETECTOR_STATE_ACTIVITY) {
        if(level >= 1) {
       //     apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Activity Detector ----DETECTOR_STATE_ACTIVITY--------2222-------- [%"APR_SIZE_T_FMT"]",level);
            detector->duration += CODEC_FRAME_TIME_BASE;
        } else {
         //   apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Activity Detector ----DETECTOR_STATE_INACTIVITY_TRANSITION---------------- [%"APR_SIZE_T_FMT"]",level);
            mpf_activity_detector_state_change(detector,DETECTOR_STATE_INACTIVITY_TRANSITION);
        }
    }
    else if(detector->state == DETECTOR_STATE_INACTIVITY_TRANSITION) {
        if(level >= 1) {
            mpf_activity_detector_state_change(detector,DETECTOR_STATE_ACTIVITY);
        }
        else {
            detector->duration += CODEC_FRAME_TIME_BASE;
            if(detector->duration >= detector->silence_timeout) {
           //     apt_log(APT_LOG_MARK,APT_PRIO_INFO,"Activity Detector ----DETECTOR_STATE_INACTIVITY---------------- [%"APR_SIZE_T_FMT"]",level);
                det_event = MPF_DETECTOR_EVENT_INACTIVITY;
                mpf_activity_detector_state_change(detector,DETECTOR_STATE_INACTIVITY);
            }
        }
    }
    return det_event;
}

/** Process current frame */
/*MPF_DECLARE(mpf_detector_event_e) mpf_activity_detector_process(mpf_activity_detector_t *detector, const mpf_frame_t *frame)
{
	mpf_detector_event_e det_event = MPF_DETECTOR_EVENT_NONE;

	apr_size_t level = 0;
	if((frame->type & MEDIA_FRAME_TYPE_AUDIO) == MEDIA_FRAME_TYPE_AUDIO) {
		level = mpf_activity_detector_level_calculate(frame);
#if 0
		apt_log(MPF_LOG_MARK,APT_PRIO_INFO,"Activity Detector [%"APR_SIZE_T_FMT"],[%"APR_SIZE_T_FMT"]",level,detector->level_threshold);
	} else {
		apt_log(MPF_LOG_MARK,APT_PRIO_INFO,"Activity Detector [%"APR_SIZE_T_FMT"][%"APR_SIZE_T_FMT"],frame type [%d]",level,detector->level_threshold,frame->type);
#endif
	}

	if(detector->state == DETECTOR_STATE_INACTIVITY) {
		if(level >= detector->level_threshold) {
			mpf_activity_detector_state_change(detector,DETECTOR_STATE_ACTIVITY_TRANSITION);
		}
		else {
			detector->duration += CODEC_FRAME_TIME_BASE;
			if(detector->duration >= detector->noinput_timeout) {
				det_event = MPF_DETECTOR_EVENT_NOINPUT;
			}
		}
	}
	else if(detector->state == DETECTOR_STATE_ACTIVITY_TRANSITION) {
		if(level >= detector->level_threshold) {
			detector->duration += CODEC_FRAME_TIME_BASE;
			if(detector->duration >= detector->speech_timeout) {
				det_event = MPF_DETECTOR_EVENT_ACTIVITY;
				mpf_activity_detector_state_change(detector,DETECTOR_STATE_ACTIVITY);
			}
		}
		else {
			mpf_activity_detector_state_change(detector,DETECTOR_STATE_INACTIVITY);
		}
	}
	else if(detector->state == DETECTOR_STATE_ACTIVITY) {
		if(level >= detector->level_threshold) {
			detector->duration += CODEC_FRAME_TIME_BASE;
		}
		else {
			mpf_activity_detector_state_change(detector,DETECTOR_STATE_INACTIVITY_TRANSITION);
		}
	}
	else if(detector->state == DETECTOR_STATE_INACTIVITY_TRANSITION) {
		if(level >= detector->level_threshold) {
			mpf_activity_detector_state_change(detector,DETECTOR_STATE_ACTIVITY);
		}
		else {
			detector->duration += CODEC_FRAME_TIME_BASE;
			if(detector->duration >= detector->silence_timeout) {
				det_event = MPF_DETECTOR_EVENT_INACTIVITY;
				mpf_activity_detector_state_change(detector,DETECTOR_STATE_INACTIVITY);
			}
		}
	}

	return det_event;
}*/