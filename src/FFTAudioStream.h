//  Copyright © 2016 Fatih Gazimzyanov. Contacts: virgil7g@gmail.com

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//          http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

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

    /**
     *  @param isApplied if true temporary processing vector applies to base signal.
     *  @warning Make sure that applyFilterToSpectrum() is true.
     * */
    void applyFilteredSignalToSound(bool isApplied);

    /**
     *  @param isApplied if true filter applies to spectrum (not yet to sound).
     * */
    void applyFilterToSpectrum(bool isApplied);

    /**
     *  Get some (SAMPLES_TO_STREAM) samples from base stream to temporary processing vector.
     * */
    void getStreamSamples();

    /**
    *  Generates filter vector which will be applied to sample spectrum according to lowFilterValue and highFilterValue.
    * */
    void generateFilterVector();

    float highFilterValue;
    float lowFilterValue;
    std::size_t currentSample;
    std::vector<sf::Int16> samplesVector;
    std::vector<complex> currentSampleWaveVector;
    std::vector<complex> currentSampleSpectrumVector;
    std::vector<complex> currentSampleCleanSpectrumVector;
    std::vector<complex> filterShortComplexVector;
    std::vector<complex> filteredWaveDataVector;
    complex temporaryShortComplex;
};


#endif //SOUND_VISUALIZER_FFTAUDIOSTREAM_H
