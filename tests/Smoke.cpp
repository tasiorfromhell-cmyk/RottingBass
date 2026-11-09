#include <juce_audio_processors/juce_audio_processors.h>
#include <iostream>
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
int main() {
 auto p=std::unique_ptr<juce::AudioProcessor>(createPluginFilter());
 if(p->getName()!="RottingBass" || p->getParameters().size()!=4) return 1;
 auto* gain=dynamic_cast<juce::RangedAudioParameter*>(p->getParameters()[3]);
 if(!gain) return 2;
 gain->setValueNotifyingHost(gain->convertTo0to1(-9.5f));
 std::vector<float> before; for(auto* v:p->getParameters())before.push_back(v->getValue());
 juce::MemoryBlock state; p->getStateInformation(state);
 gain->setValueNotifyingHost(gain->convertTo0to1(0));p->setStateInformation(state.getData(),(int)state.getSize());
 for(int i=0;i<p->getParameters().size();++i)if(std::abs(before[(size_t)i]-p->getParameters()[i]->getValue())>1e-6f)return 6;
 if(std::abs(gain->convertFrom0to1(gain->getValue())+9.5f)>0.01f) return 3;
 for(int channels : {1,2}) {
  auto layout=p->getBusesLayout();auto set=channels==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo();
  layout.inputBuses.set(0,set);layout.outputBuses.set(0,set);
  if(!p->setBusesLayout(layout))return 4;
  p->prepareToPlay(48000,128);juce::AudioBuffer<float> b(channels,128);juce::MidiBuffer midi;
  for(int c=0;c<channels;++c)for(int n=0;n<128;++n)b.setSample(c,n,0.5f);
  p->processBlock(b,midi);
  for(int c=0;c<channels;++c)for(int n=0;n<128;++n)if(std::abs(b.getSample(c,n)-0.5f*std::pow(10.f,-9.5f/20.f))>1e-6f)return 5;
  p->releaseResources();
 }
 std::cout<<"PASS: mono/stereo, Gain, state recall. No REAPER or RBass validation.\n";
}
