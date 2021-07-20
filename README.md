# Turkish Formant Speech Synthesizer

The new project can be found [here](https://gitlab.com/sausagenoods/lapic).

This is a simple speech synthesizer that uses LPC decoding. Inspired by [Bisqwit's speech synthesis series](https://youtu.be/Jcymn3RGkF4).

It's possible for it to speak other languages in Turkish-ified form with a funny accent. Here is a demo of the synthesizer singing Daisy Bell.

[![YoutubeVideo](https://img.youtube.com/vi/GelelqJ59tU/0.jpg)](https://youtu.be/GelelqJ59tU)

## Usage
```
synth -s "text" -o out.wav
-s <text>
-o <path>: WAV output to file
-f: use the female voice
-m: use the male voice (experimental, very loud)
-t<tempo value>
-p<pitch value>
-g<gain value>
-a<aspiration value>
```

### Examples
Female: `bin/synth -s "Merhaba ben bir konuşma sentezleyiciyim" -t1.5`

Male: `bin/synth -m -s "Ben de bir konuşma sentezleyiciyim_" -t1.7`

Whisper: `bin/synth -s "uuuu spuuki" -a0.002 -g1 -t1.5`

## TÜBİTAK
This project was my submission to TUBITAK's High School Students Research Projects Competition. The project was rated 81.3 out of 100 by the zookeepers.

Bu proje ile 2020 Lise Öğrencileri Araştırma Projeleri Yarışması'na katıldım. Proje puanını ve raporumu [burada](https://gitlab.com/sausagenoods/turkish-speech-synth/-/blob/master/misc/tubitak/) bulabilirsiniz.
