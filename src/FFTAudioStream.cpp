//
// Created by ViRGiL7 on 13.10.2016.
//

#include <SFML/Audio.hpp>
#include "FFTAudioStream.h"
#include "fft.h"

#define SONG_FILE "D:/Code/Projects/sound-visualizer/BTO.ogg"

void FFTAudioStream::load(const sf::SoundBuffer &buffer) {
    // extract the audio samples from the sound buffer to our own container
    m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

    highFilterValue = 50;
    lowFilterValue = 30;
    complex temporaryComplex;
    filterShortComplexArray.resize(SAMPLES_TO_STREAM / 4);
    for (int i = 0; i < SAMPLES_TO_STREAM / 4; i++) {
        temporaryComplex = (i > lowFilterValue && i < highFilterValue) ? 1.0 : 0.0;
        filterShortComplexArray[i] = temporaryComplex;
    }

    currentSampleWaveVector.resize(SAMPLES_TO_STREAM);

    // reset the current playing position
    m_currentSample = 0;

    // initialize the base class
    initialize(buffer.getChannelCount(), buffer.getSampleRate());
}

bool FFTAudioStream::onGetData(sf::SoundStream::Chunk &data) {
    // number of samples to stream every time the function is called;
    // in a more robust implementation, it should be a fixed
    // amount of time rather than an arbitrary number of samples

    getStreamSamples();

    CFFT::Forward(currentSampleWaveVector.data(), SAMPLES_TO_STREAM);

    applyFilterToSpectrum(true);

    currentSampleSpectrumVector = currentSampleWaveVector;

    CFFT::Inverse(currentSampleWaveVector.data(), SAMPLES_TO_STREAM);

    filteredWaveDataVector = currentSampleWaveVector;

    applyFilteredSignalToSound(true);

    // set the pointer to the next audio samples to be played
    data.samples = &m_samples[m_currentSample];

    // have we reached the end of the sound?
    if (m_currentSample + SAMPLES_TO_STREAM <= m_samples.size()) {
        // end not reached: stream the samples and continue
        data.sampleCount = SAMPLES_TO_STREAM;
        m_currentSample += SAMPLES_TO_STREAM;
        return true;
    } else {
        // end of stream reached: stream the remaining samples and stop playback
        data.sampleCount = m_samples.size() - m_currentSample;
        m_currentSample = m_samples.size();
        return false;
    }
}

void FFTAudioStream::getStreamSamples() {
    for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
        temporaryShortComplex = m_samples[m_currentSample + i];
        currentSampleWaveVector[i] = temporaryShortComplex;
    }
}

void FFTAudioStream::applyFilterToSpectrum(bool isApplied) {
    if (isApplied) {
        for (int T = 0; T < 4; T++) {
            for (int i = 0; i < SAMPLES_TO_STREAM / 4; i++) {
                currentSampleWaveVector[SAMPLES_TO_STREAM / 4 * T + i] =
                        currentSampleWaveVector[SAMPLES_TO_STREAM / 4 * T + i] *
                        filterShortComplexArray[(T % 2 == 0 ? i : SAMPLES_TO_STREAM / 4 - i)];
            }
        }
    }
}

void FFTAudioStream::applyFilteredSignalToSound(bool isApplied) {
    if (isApplied) {
        for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
            temporaryShortComplex = currentSampleWaveVector[i];
            m_samples[m_currentSample + i] = (sf::Int16) temporaryShortComplex.re();
        }
    }
}

void FFTAudioStream::onSeek(sf::Time timeOffset) {
    // compute the corresponding sample index according to the sample rate and channel count
    m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}

const std::vector<complex> &FFTAudioStream::getCurrentSampleWaveVector() const {
    return filteredWaveDataVector;
}

const std::vector<complex> &FFTAudioStream::getCurrentSampleSpectrumVector() const {
    return currentSampleSpectrumVector;
}

int FFTAudioStream::getLowFilterValue() {
    return lowFilterValue;
}

void FFTAudioStream::setLowFilterValue(int lowFilterValue) {
    FFTAudioStream::lowFilterValue = lowFilterValue;
}

int FFTAudioStream::getHighFilterValue() {
    return highFilterValue;
}

void FFTAudioStream::setHighFilterValue(int highFilterValue) {
    FFTAudioStream::highFilterValue = highFilterValue;
}
