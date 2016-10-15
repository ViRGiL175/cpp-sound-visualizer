//
// Created by ViRGiL7 on 13.10.2016.
//

#ifndef SOUND_VISUALIZER_FFTAUDIOSTREAM_H
#define SOUND_VISUALIZER_FFTAUDIOSTREAM_H


static const int SAMPLES_TO_STREAM = 1024 * 2;
static const int LOW_FILTER_VALUE = 20;
static const int HIGH_FILTER_VALUE = 30;

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Time.hpp>
#include "complex.h"

class FFTAudioStream : public sf::SoundStream {

public:

    void load(const sf::SoundBuffer &buffer);

    const std::vector<complex> &getWaveDataVector() const;

private:

    virtual bool onGetData(Chunk &data);

    virtual void onSeek(sf::Time timeOffset);

    void applyFilteredSignalToSound(bool isApplied);

    void applyFilterToSpectrum(bool isApplied);

    void getStreamSamples();

    std::size_t m_currentSample;
    std::vector<sf::Int16> m_samples;
    std::vector<complex> currentSampleVector;
    std::vector<complex> filterShortComplexArray;
    std::vector<complex> filteredWaveDataVector;
    complex temporaryShortComplex;

};


#endif //SOUND_VISUALIZER_FFTAUDIOSTREAM_H
