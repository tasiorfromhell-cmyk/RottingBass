#include "AudioChecks.h"
int main(int argc,char** argv) {
 try {
  juce::ScopedJuceInitialiser_GUI gui;
  checks::require(argc==2,"Usage: RottingBassHostTest <absolute RottingBass.vst3 bundle path>");
  juce::VST3PluginFormat format;juce::OwnedArray<juce::PluginDescription> types;
  format.findAllTypesForFile(types,juce::String::fromUTF8(argv[1]));checks::require(types.size()==1,"VST3 scan failed");
  const auto& description=*types[0];checks::require(description.name=="RottingBass","Host product name");
  checks::require(description.manufacturerName=="Rottingpitstufio","Host vendor");
  juce::String error;auto p=format.createInstanceFromDescription(description,48000,257,error);
  if(!p)throw std::runtime_error("VST3 instantiation: "+error.toStdString());
  checks::run(*p);std::cout<<"PASS VST3 loaded in JUCE test host (not REAPER).\n";return 0;
 } catch(const std::exception& e) { std::cerr<<"FAIL "<<e.what()<<"\n";return 1; }
}
