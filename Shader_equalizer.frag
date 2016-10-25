//  Copyright © 2016 Fatih Gazimzyanov. Contacts: virgil7g@gmail.com
//  Copyright © 2013 Tyler Tesch.       Contacts: https://www.youtube.com/user/Advenio4821

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

#version 150 core
uniform vec3  iResolution;
uniform float iGlobalTime;
uniform float spectrum[256];
uniform float waveData[256];
in vec4 gl_FragCoord;
out vec4 gl_FragColor;

void InterpolateValue(in float index, out float value) {
	float norm = 255.0 / iResolution.x * index;
	int Floor = int(floor(norm));
    value = waveData[Floor];
}

#define THICKNESS 0.02
void main()
{
	float time = iGlobalTime;
	float x = gl_FragCoord.x / iResolution.x;
	float y = gl_FragCoord.y / iResolution.y ;

	float wave = 0;
	InterpolateValue(x * iResolution.x, wave);

	if (y < wave) {
	    gl_FragColor = vec4(
	        0.6 + 0.4 * sin(time + 3.14), 0.6 + 0.4 * sin(time + 3.14 * 2), 0.6 + 0.4 * sin(time + 0.7) , 0);
	} else {
		gl_FragColor = vec4(0, 0, 0, 0);
	};
}