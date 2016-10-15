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

    complex temporaryComplex;
    filterShortComplexArray.resize(SAMPLES_TO_STREAM);
    for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
        temporaryComplex = (i > LOW_FILTER_VALUE && i < HIGH_FILTER_VALUE) ? 1.0 : 0.0;
        filterShortComplexArray[i] = temporaryComplex;
    }

    currentSampleVector.resize(SAMPLES_TO_STREAM);

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

    CFFT::Forward(currentSampleVector.data(), SAMPLES_TO_STREAM);

    applyFilterToSpectrum(true);

    CFFT::Inverse(currentSampleVector.data(), SAMPLES_TO_STREAM);

    filteredWaveDataVector = currentSampleVector;

    applyFilteredSignalToSound(false);

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
        currentSampleVector[i] = temporaryShortComplex;
    }
}

void FFTAudioStream::applyFilterToSpectrum(bool isApplied) {
    if (isApplied) {
        for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
            currentSampleVector[i] = currentSampleVector[i] * filterShortComplexArray[i];
        }
    }
}

void FFTAudioStream::applyFilteredSignalToSound(bool isApplied) {
    if (isApplied) {
        for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
            temporaryShortComplex = currentSampleVector[i];
            m_samples[m_currentSample + i] = (sf::Int16) temporaryShortComplex.re();
        }
    }
}

void FFTAudioStream::onSeek(sf::Time timeOffset) {
    // compute the corresponding sample index according to the sample rate and channel count
    m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}

const std::vector<complex> &FFTAudioStream::getWaveDataVector() const {
    return filteredWaveDataVector;
}
