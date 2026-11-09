#pragma once
#include <juce_dsp/juce_dsp.h>
#include <array>
// Original algorithm, not an identified RBass model. No allocations in process().
class BassEngine {
 struct Biquad {
  double b0=1,b1=0,b2=0,a1=0,a2=0,z1=0,z2=0;
  void reset() { z1=z2=0; }
  void configure(double hz,double sr,double q,bool high=false) {
   const double w=2*juce::MathConstants<double>::pi*hz/sr, c=std::cos(w), alpha=std::sin(w)/(2*q), inv=1/(1+alpha);
   b0=(high?(1+c):(1-c))*0.5*inv;b1=(high?-(1+c):(1-c))*inv;b2=b0;a1=-2*c*inv;a2=(1-alpha)*inv;
  }
  float tick(float input) { const double y=b0*input+z1;z1=b1*input-a1*y+z2;z2=b2*input-a2*y;return (float)y; }
 };
 struct Channel { Biquad low, wetHigh, wetLow1, wetLow2; };
 std::array<Channel,2> channels;
 std::unique_ptr<juce::dsp::Oversampling<float>> oversampler;
 juce::SmoothedValue<float> freq, amount, removeBass, gain;
 double oversampledRate=192000; int maxBlock=0, channelCount=0; unsigned int coefficientCounter=0;
 void updateFilters(float f) {
  for(auto& ch:channels) {
   ch.low.configure(f,oversampledRate,0.7071067811865476);
   ch.wetHigh.configure(f*0.75,oversampledRate,0.7071067811865476,true);
   const double ceiling=juce::jmin(6000.0,(double)f*8.0);
   ch.wetLow1.configure(ceiling,oversampledRate,0.541196100146197);
   ch.wetLow2.configure(ceiling,oversampledRate,1.306562964876377);
  }
 }
public:
 void prepare(double sr,int block,int count,float f,float i,bool original,float g) {
  maxBlock=juce::jmax(1,block);channelCount=count;oversampledRate=sr*4;
  oversampler=std::make_unique<juce::dsp::Oversampling<float>>((size_t)count,2,juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR,true,true);
  oversampler->initProcessing((size_t)maxBlock);oversampler->reset();
  for(auto* s:{&freq,&amount,&removeBass,&gain})s->reset(oversampledRate,0.02);
  freq.setCurrentAndTargetValue(f);amount.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(i));
  removeBass.setCurrentAndTargetValue(original?0.f:1.f);gain.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(g));
  for(auto& ch:channels)for(auto* filter:{&ch.low,&ch.wetHigh,&ch.wetLow1,&ch.wetLow2})filter->reset();
  coefficientCounter=0;updateFilters(f);
 }
 int latency() const { return oversampler?(int)std::lround(oversampler->getLatencyInSamples()):0; }
 void process(juce::AudioBuffer<float>& audio,float f,float intensity,bool original,float outputGain) {
  if(!oversampler || audio.getNumSamples()==0)return;
  jassert(audio.getNumChannels()==channelCount);
  if(audio.getNumChannels()!=channelCount) { audio.clear();return; }
  freq.setTargetValue(f);amount.setTargetValue(juce::Decibels::decibelsToGain(intensity));
  removeBass.setTargetValue(original?0.f:1.f);gain.setTargetValue(juce::Decibels::decibelsToGain(outputGain));
  // Hosts may exceed prepareToPlay's block hint. Chunk without reallocating.
  juce::dsp::AudioBlock<float> all(audio);
  for(size_t offset=0;offset<all.getNumSamples();offset+=(size_t)maxBlock) {
   auto part=all.getSubBlock(offset,juce::jmin((size_t)maxBlock,all.getNumSamples()-offset));
   auto up=oversampler->processSamplesUp(part);
   for(size_t n=0;n<up.getNumSamples();++n) {
    const float cutoff=freq.getNextValue(), wetGain=amount.getNextValue(), remove=removeBass.getNextValue(), level=gain.getNextValue();
    if((coefficientCounter++ & 15)==0)updateFilters(cutoff);
    for(size_t c=0;c<up.getNumChannels();++c) {
     auto& ch=channels[c];const float input=up.getSample((int)c,(int)n), low=ch.low.tick(input);
     const float limited=std::tanh(3.f*low);
     // Full-wave branch supplies even harmonics; soft saturation residual odd.
     const float generated=0.5f*std::abs(limited)+0.2f*(limited-3.f*low);
     const float harmonics=ch.wetLow2.tick(ch.wetLow1.tick(ch.wetHigh.tick(generated)));
     up.setSample((int)c,(int)n,(input-remove*low+wetGain*harmonics)*level);
    }
   }
   oversampler->processSamplesDown(part);
  }
 }
};
