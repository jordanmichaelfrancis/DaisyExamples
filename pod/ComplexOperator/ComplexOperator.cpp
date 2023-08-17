#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod   hw;
VariableSawOscillator varosc;
Oscillator sineosc;
Parameter  p_freq;
Parameter  p_pw;
Parameter p_waveshape;

static float freq;
static float pw;
static float waveshape;
float        sig;

static void AudioCallback(AudioHandle::InterleavingInputBuffer  in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t                                size)
{
    hw.ProcessDigitalControls();

    freq = p_freq.Process();
    pw = p_pw.Process();
    waveshape = p_waveshape.Process();
    
    sineosc.SetFreq(freq);
    varosc.SetFreq(freq);
    varosc.SetPW(pw);

    // Audio Loop
    for(size_t i = 0; i < size; i += 2)
    {
        // Process
        sig        = (varosc.Process() * waveshape + sineosc.Process() * (1.f - waveshape)) ;
        out[i]     = sig;
        out[i + 1] = sig;
    }
}

void InitSynth(float samplerate)
{
    p_freq.Init(hw.knob1, 100, 10000, Parameter::EXPONENTIAL);
    // p_pw.Init(hw.knob2, -1, 1, Parameter::LINEAR);
    p_waveshape.Init(hw.knob2, 0, 1, Parameter::LINEAR);

    sineosc.Init(samplerate);
    sineosc.SetWaveform(Oscillator::WAVE_SIN);
    sineosc.SetFreq(110.f);
    sineosc.SetAmp(1.f);

    varosc.Init(samplerate);
    varosc.SetFreq(110.f);
    varosc.SetWaveshape(1);

}

int main(void)
{
    float samplerate;

    // Init everything
    hw.Init();
    hw.SetAudioBlockSize(4);
    samplerate = hw.AudioSampleRate();

    InitSynth(samplerate);

    // start callbacks
    hw.StartAdc();
    hw.StartAudio(AudioCallback);

    while(1) {}
}