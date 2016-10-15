//
// Created by ViRGiL7 on 13.10.2016.
//

#ifndef SOUND_VISUALIZER_FFTAUDIOSTREAM_H
#define SOUND_VISUALIZER_FFTAUDIOSTREAM_H

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/System/Time.hpp>
#include "complex.h"

class FFTAudioStream : public sf::SoundStream {

public:

    static const int SAMPLES_TO_STREAM = 1024 * 2;

    void load(const sf::SoundBuffer &buffer);

    const std::vector<complex> &getCurrentSampleWaveVector() const;

    const std::vector<complex> &getCurrentSampleSpectrumVector() const;

    int getLowFilterValue();

    void setLowFilterValue(int lowFilterValue);

    int getHighFilterValue();

    void setHighFilterValue(int highFilterValue);

private:

    virtual bool onGetData(Chunk &data);

    virtual void onSeek(sf::Time timeOffset);

    void applyFilteredSignalToSound(bool isApplied);

    void applyFilterToSpectrum(bool isApplied);

    void getStreamSamples();

    int highFilterValue;
    int lowFilterValue;
    std::size_t m_currentSample;
    std::vector<sf::Int16> m_samples;
    std::vector<complex> currentSampleWaveVector;
    std::vector<complex> currentSampleSpectrumVector;
    std::vector<complex> filterShortComplexArray;
    std::vector<complex> filteredWaveDataVector;
    complex temporaryShortComplex;

};


#endif //SOUND_VISUALIZER_FFTAUDIOSTREAM_H
