# sound-visualizer
OpenGL visualization and FFT spectrum filtering

![Wave data visualization](http://i.imgur.com/7w5fhosl.png "Wave data visualization")

![Spectrum visualization](http://i.imgur.com/bKBndETl.png "Spectrum visualization")

![Bars visualization](http://i.imgur.com/Bh3YW8ml.gif "Bars visualization")

[Bars visualization animation](http://i.imgur.com/Bh3YW8m.gifv)

# What is it?
This is an OpenGL visualizer for sound files.

It uses SFML library for window creating and sound playing.

It has 3 types of visualization:
- Wave data;
- Spectrum;
- Bars visualization.

Also you can apply FFT (Fast Fourier Transform) filter to sound and it will be shown on visualization.

And it's my first experience in C++.

# For what?
For education purposes.

I made it for my DSP university course.

# Any reference?
Of course.

https://www.youtube.com/watch?v=kf1m1KqmCJg - start point of all my work. Thanks Tyler Tesch for shaders example and openGL base.

http://www.librow.com/articles/article-10 - I used many libraries and FFT methods, but, for some reasons, choose this one. Thanks Sergey Chernenko for FFT code and great articles.

# How to use it?
Just drag&drop your music files onto *.exe. You can use ogg, wav, flac, aiff, au and many more SFML [supported](http://www.sfml-dev.org/documentation/2.0/classsf_1_1Music.php) audio formats (but not mp3).

Then, according to in-code documentation:
```
/**
 *  Main function
 *
 *  @Controls
 *  P - play/pause;
 *  M - switch visualization mode;
 *  R - reload the shader;
 *  T - change track;
 *  Q - increase bottom filter threshold;
 *  Z - decrease bottom filter threshold;
 *  W - increase upper filter threshold;
 *  X - decrease upper filter threshold;
 *  Escape - exit.
 * */
 ```
 
# Whats now?
Be free to make forks and pull requests :)
