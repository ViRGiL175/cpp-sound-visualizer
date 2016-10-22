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

    static const int SAMPLES_TO_STREAM = 1024;

    void load(const sf::SoundBuffer &buffer);

    const std::vector<complex> &getCurrentSampleWaveVector() const;

    const std::vector<complex> &getCurrentSampleSpectrumVector() const;

    const std::vector<complex> &getCurrentSampleCleanSpectrumVector() const;

    float getLowFilterValue();

    void setLowFilterValue(float lowFilterValue);

    float getHighFilterValue();

    void setHighFilterValue(float highFilterValue);

private:

    virtual bool onGetData(Chunk &data);

    virtual void onSeek(sf::Time timeOffset);

    void applyFilteredSignalToSound(bool isApplied);

    void applyFilterToSpectrum(bool isApplied);

    void getStreamSamples();

    void generateFilterVector();

    float highFilterValue;
    float lowFilterValue;
    std::size_t m_currentSample;
    std::vector<sf::Int16> m_samples;
    std::vector<complex> currentSampleWaveVector;
    std::vector<complex> currentSampleSpectrumVector;
    std::vector<complex> currentSampleCleanSpectrumVector;
    std::vector<complex> filterShortComplexVector;
    std::vector<complex> filteredWaveDataVector;
    complex temporaryShortComplex;
};


#endif //SOUND_VISUALIZER_FFTAUDIOSTREAM_H
