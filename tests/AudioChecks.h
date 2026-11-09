#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace checks {
inline void require(bool ok,const char* message) { if(!ok)throw std::runtime_error(message); }
inline void parameter(juce::AudioProcessor& p,const juce::String& name,float normalized) {
 for(auto* v:p.getParameters())if(v->getName(100)==name) { v->setValueNotifyingHost(normalized);return; }
 throw std::runtime_error("Missing parameter: "+name.toStdString());
}
struct Settings { float freq=66,intensity=6.9f,original=1,gain=-9.5f; };
inline void settings(juce::AudioProcessor& p,Settings s) {
 parameter(p,"Freq",(s.freq-32)/224);parameter(p,"Intensity",(s.intensity+24)/48);
 parameter(p,"Original Bass",s.original);parameter(p,"Gain",(s.gain+48)/60);
}
inline std::vector<float> render(juce::AudioProcessor& p,Settings s,double sr=48000,int channels=2,int block=257,bool onlyLeft=false,bool silence=false,bool automation=false,int prepareHint=0) {
 p.releaseResources();auto layout=p.getBusesLayout();auto set=channels==1?juce::AudioChannelSet::mono():juce::AudioChannelSet::stereo();
 require(layout.inputBuses.size()==1 && layout.outputBuses.size()==1,"Bus count");layout.inputBuses.set(0,set);layout.outputBuses.set(0,set);
 require(p.setBusesLayout(layout),"Unsupported mono/stereo layout");
 settings(p,s);p.setRateAndBufferSizeDetails(sr,prepareHint?prepareHint:block);p.prepareToPlay(sr,prepareHint?prepareHint:block);
 const int N=(int)sr;std::vector<float> output((size_t)N*(size_t)channels);juce::AudioBuffer<float> buffer(channels,block);juce::MidiBuffer midi;
 for(int pos=0;pos<N;pos+=block) {
  const int count=std::min(block,N-pos);buffer.setSize(channels,count,false,false,true);
  if(automation)settings(p,{pos%2?256.f:32.f,pos%3?24.f:-24.f,pos%5?0.f:1.f,pos%7?-48.f:12.f});
  for(int c=0;c<channels;++c)for(int n=0;n<count;++n) {
   const double t=(pos+n)/sr;const float x=silence || (onlyLeft && c==1)?0.f:0.2f*(float)std::sin(2*juce::MathConstants<double>::pi*41*t);
   buffer.setSample(c,n,x);
  }
  p.processBlock(buffer,midi);
  for(int c=0;c<channels;++c)for(int n=0;n<count;++n) { const float v=buffer.getSample(c,n);require(std::isfinite(v),"NaN/Inf output");output[(size_t)(pos+n)*(size_t)channels+(size_t)c]=v; }
 }
 return output;
}
inline double rms(const std::vector<float>& x) { double sum=0;for(size_t n=x.size()/2;n<x.size();++n)sum+=(double)x[n]*x[n];return std::sqrt(sum/(double)(x.size()-x.size()/2)); }
inline double difference(const std::vector<float>& x,const std::vector<float>& y) { require(x.size()==y.size(),"Shape");std::vector<float> e(x.size());for(size_t i=0;i<x.size();++i)e[i]=x[i]-y[i];return rms(e); }
inline double harmonic(const std::vector<float>& x,double sr,int order) {
 double real=0,imag=0,windowSum=0;const size_t start=x.size()/2;
 for(size_t n=start;n<x.size();++n) { const double window=0.5-0.5*std::cos(2*juce::MathConstants<double>::pi*(double)(n-start)/(double)(x.size()-start-1));const double w=2*juce::MathConstants<double>::pi*41*order*(double)n/sr;real+=window*x[n]*std::cos(w);imag-=window*x[n]*std::sin(w);windowSum+=window; }
 return 2*std::hypot(real,imag)/windowSum;
}
inline void run(juce::AudioProcessor& p) {
 require(p.getName()=="RottingBass","Product name");require(p.getParameters().size()>=4,"Parameter count");
 const Settings base{};const auto a=render(p,base);
 for(auto s:{Settings{160,6.9f,1,-9.5f},Settings{66,-12,1,-9.5f},Settings{66,6.9f,0,-9.5f},Settings{66,6.9f,1,-3.5f}}) {
  const double error=difference(a,render(p,s));std::cout<<"control difference RMS "<<error<<"\n";require(error>1e-5,"Control does not affect audio");
 }
 const auto boosted=render(p,{66,6.9f,1,-3.5f});require(std::abs(rms(boosted)/rms(a)-std::pow(10.,6./20))<1e-4,"Gain scale");
 auto mono=render(p,base,48000,1);const double h2=harmonic(mono,48000,2);std::cout<<"H2 amplitude "<<h2<<"\n";require(h2>0.001,"No generated second harmonic");
 settings(p,{83,-4,0,-3});juce::MemoryBlock saved;p.getStateInformation(saved);require(saved.getSize()>0,"Empty state");
 std::vector<float> values;for(auto* v:p.getParameters())values.push_back(v->getValue());settings(p,base);p.setStateInformation(saved.getData(),(int)saved.getSize());
 for(int i=0;i<p.getParameters().size();++i)require(std::abs(values[(size_t)i]-p.getParameters()[i]->getValue())<1e-5f,"State recall");
 const auto recallA=render(p,{83,-4,0,-3});const auto recallB=render(p,{83,-4,0,-3});require(difference(recallA,recallB)<1e-7,"Repeatability");
 for(double sr:{44100.,48000.,96000.})for(int channels:{1,2}) {const auto v=render(p,base,sr,channels,127);require(rms(v)>0.001,"Silent output");}
 const auto left=render(p,base,48000,2,127,true);double leak=0;for(size_t n=1;n<left.size();n+=2)leak=std::max(leak,std::abs((double)left[n]));require(leak<1e-12,"Stereo crossfeed");
 const auto silent=render(p,base,48000,2,127,false,true);require(rms(silent)==0,"Silence produces audio");
 const auto b17=render(p,base,48000,2,17),b512=render(p,base,48000,2,512);require(difference(b17,b512)<1e-7,"Block-dependent DSP");
 const auto oversized=render(p,base,48000,2,1024,false,false,false,64);require(difference(oversized,b512)<1e-7,"Block hint overflow/chunking");
 render(p,base,48000,2,31,false,false,true);p.releaseResources();
 std::cout<<"PASS mono/stereo, 44.1/48/96k, four controls, gain, harmonic, recall, repeatability, channel isolation, silence, block sizes, automation. Latency="<<p.getLatencySamples()<<"\n";
}
}
