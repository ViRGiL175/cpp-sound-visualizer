//
// Created by ViRGiL7 on 13.10.2016.
//

#include <SFML/Audio.hpp>
#include <tools/TextPlot.h>
#include <transform/FftFactory.h>
#include "FFTAudioStream.h"

#define SONG_FILE "D:/Code/Projects/sound-visualizer/BTO.ogg"

void FFTAudioStream::load(const sf::SoundBuffer &buffer) {
    // extract the audio samples from the sound buffer to our own container
    m_samples.assign(buffer.getSamples(), buffer.getSamples() + buffer.getSampleCount());

    ShortComplex shortComplex;
    shortComplex.im = 0.0;

    filterShortComplexArray.resize(SAMPLES_TO_STREAM);
    for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
//        shortComplex.re = (i > SAMPLES_TO_STREAM / 2) ? 1.0 : 0.0;
        shortComplex.re = 1.0;
        filterShortComplexArray[i] = shortComplex;
    }

    currentSampleVector.resize(SAMPLES_TO_STREAM);

    // reset the current playing position
    m_currentSample = 0;

    temporaryShortComplex.im = 0.0;

    // initialize the base class
    initialize(buffer.getChannelCount(), buffer.getSampleRate());
}

bool FFTAudioStream::onGetData(sf::SoundStream::Chunk &data) {
    // number of samples to stream every time the function is called;
    // in a more robust implementation, it should be a fixed
    // amount of time rather than an arbitrary number of samples

    for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
        temporaryShortComplex.re = m_samples[m_currentSample + i];
        currentSampleVector[i] = temporaryShortComplex;
    }

    fft(currentSampleVector.data(), SAMPLES_TO_STREAM, false);

    std::transform(
            std::begin(currentSampleVector),
            std::end(currentSampleVector),
            std::begin(filterShortComplexArray),
            std::begin(currentSampleVector),
            [](ShortComplex x, ShortComplex y) { return x * y; }
    );

    fft(currentSampleVector.data(), SAMPLES_TO_STREAM, true);

    applyFilteredSignalToSound();

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

const std::vector<ShortComplex> &FFTAudioStream::getCurrentSampleVector() const {
    return currentSampleVector;
}

void FFTAudioStream::applyFilteredSignalToSound() {
    for (int i = 0; i < SAMPLES_TO_STREAM; i++) {
        temporaryShortComplex.re = currentSampleVector[i].re;
        m_samples[m_currentSample + i] = (sf::Int16) temporaryShortComplex.re;
    }
}

void FFTAudioStream::onSeek(sf::Time timeOffset) {
    // compute the corresponding sample index according to the sample rate and channel count
    m_currentSample = static_cast<std::size_t>(timeOffset.asSeconds() * getSampleRate() * getChannelCount());
}

