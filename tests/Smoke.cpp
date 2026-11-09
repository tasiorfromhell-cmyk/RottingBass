#include "AudioChecks.h"
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter();
int main() { try { auto p=std::unique_ptr<juce::AudioProcessor>(createPluginFilter());checks::run(*p);return 0; } catch(const std::exception& e) { std::cerr<<"FAIL "<<e.what()<<"\n";return 1; } }
