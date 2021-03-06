//
// Created by julien@macmini on 07/05/2018.
//

#ifndef ANDROIDLINKAUDIO_AUDIOENGINE_H
#define ANDROIDLINKAUDIO_AUDIOENGINE_H

#include <oboe/Oboe.h>
#include <mutex>
#include "SineGenerator.h"
#include <ableton/Link.hpp>
#include <ableton/link/HostTimeFilter.hpp>

constexpr int32_t kBufferSizeAutomatic = 0;
constexpr int32_t kMaximumChannelCount = 2 ;

enum PlayStatus {stopped, playing};

class AudioEngine: oboe::AudioStreamCallback {

public:
    AudioEngine();
    ~AudioEngine();



    // API
    void createStream();
    void enableLink(bool enableFlag);
    void playAudio(bool playFlag);
    void setLatencyMs(int latencyMs);
    void detectLatency(bool flag);
    double getLatencyMs();

private:

    // AUDIO STREAM(s)
    void createPlaybackStream();
    void setupPlaybackStreamParameters(oboe::AudioStreamBuilder *builder);
    void onErrorAfterClose(oboe::AudioStream *oboeStream, oboe::Result error);

    oboe::AudioApi mAudioApi = oboe::AudioApi::Unspecified;
    int32_t mPlaybackDeviceId = oboe::kUnspecified;
    int32_t mSampleRate;
    int32_t mChannelCount;
    int32_t mFramesPerBurst;
    double mCurrentOutputLatencyMillis = 0;
    double mDetectedOutputLatencyMillis = 0;
    int32_t mBufferSizeSelection = kBufferSizeAutomatic;
    bool mIsLatencyDetectionSupported = false;
    oboe::AudioStream *mPlayStream;
    std::unique_ptr<oboe::LatencyTuner> mLatencyTuner;
    std::mutex mRestartingLock;
    std::mutex mRestartingInputLock;
    void closeOutputStream();
    void closeInputStream();
    void restartStream();
    void restartRecStream();
    oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames);

    void setupRecStreamParameters(oboe::AudioStreamBuilder *builder);
    void createRecStream();
    oboe::AudioStream *mRecStream;

    // AUDIO ENGINE
    std::array<SineGenerator, kMaximumChannelCount> mOscillators;
    void prepareOscillators();
    oboe::Result calculateCurrentOutputLatencyMillis(oboe::AudioStream *stream, double *latencyMillis);
    PlayStatus mPlayStatus;
    void renderBarClick(float *buffer,
                        int32_t channelStride,
                        int32_t numFrames,
                        ableton::Link::SessionState sessionState,
                        std::chrono::microseconds bufferBeginAtOutput,
                        double microsPerSample);
    void renderBarClick(int16_t *buffer,
                        int32_t channelStride,
                        int32_t numFrames,
                        ableton::Link::SessionState sessionState,
                        std::chrono::microseconds bufferBeginAtOutput,
                        double microsPerSample);

    bool mPerformLatencyDetection = false;
    float* mInputBuffer = nullptr;
    int16_t * mInputBuffer_int16 = nullptr;
    void processInput(float *buffer,
                        int32_t channelStride,
                        int32_t numFrames);
    void processInput(int16_t *buffer,
                      int32_t channelStride,
                      int32_t numFrames);
    int mLatencySampleCount = 0;

    // ABLETON LINK
    ableton::Link link;
    double mSampleTime;
    ableton::link::HostTimeFilter<ableton::link::platform::Clock> mHostTimeFilter;
    double mLastBeatPhase;
    double mLastBarPhase;
    std::chrono::microseconds timeAtLastBar;
    double mQuantum = 4;

};


#endif //ANDROIDLINKAUDIO_AUDIOENGINE_H
