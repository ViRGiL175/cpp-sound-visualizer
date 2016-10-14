//
// Created by virgi on 13.10.2016.
//

#ifndef SOUND_VISUALIZER_FFTAUDIOSTREAM_H
#define SOUND_VISUALIZER_FFTAUDIOSTREAM_H


static const int SAMPLES_TO_STREAM = 1024 * 2;

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Time.hpp>
#include "Fft.h"

class FFTAudioStream : public sf::SoundStream {

public:

    void load(const sf::SoundBuffer &buffer);

    const std::vector<ShortComplex> &getCurrentSampleVector() const;

private:

    virtual bool onGetData(Chunk &data);

    virtual void onSeek(sf::Time timeOffset);

    void applyFilteredSignalToSound();

    std::vector<sf::Int16> m_samples;

    std::size_t m_currentSample;
    std::vector<ShortComplex> currentSampleVector;

    std::vector<ShortComplex> filterShortComplexArray;

    ShortComplex temporaryShortComplex;
};


#endif //SOUND_VISUALIZER_FFTAUDIOSTREAM_H
