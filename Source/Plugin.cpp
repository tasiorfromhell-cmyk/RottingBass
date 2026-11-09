#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
class Processor;
class Editor final : public juce::AudioProcessorEditor {
public:
 explicit Editor(Processor&);
 void paint(juce::Graphics&) override;
 void resized() override;
private:
 juce::Slider freq, intensity, gain;
 juce::Label fl, il, gl;
 juce::ToggleButton original { "Original Bass IN" };
 using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
 std::unique_ptr<SA> fa, ia, ga;
 std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> oa;
};
class Processor final : public juce::AudioProcessor {
public:
 Processor() : AudioProcessor(BusesProperties().withInput("Input",juce::AudioChannelSet::stereo(),true).withOutput("Output",juce::AudioChannelSet::stereo(),true)), state(*this,nullptr,"RottingBassState",layout()) {}
 static juce::AudioProcessorValueTreeState::ParameterLayout layout() {
  juce::AudioProcessorValueTreeState::ParameterLayout l;
  l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"freq",1},"Freq",juce::NormalisableRange<float>(32,256,0.1f),66));
  l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"intensity",1},"Intensity",juce::NormalisableRange<float>(-24,24,0.1f),6.9f));
  l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{"originalBass",1},"Original Bass",true));
  l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{"gain",1},"Gain",juce::NormalisableRange<float>(-48,12,0.1f),-9.5f)); return l;
 }
 const juce::String getName() const override { return "RottingBass"; }
 void prepareToPlay(double sr,int) override { smoothing.reset(sr,0.02); smoothing.setCurrentAndTargetValue(juce::Decibels::decibelsToGain(state.getRawParameterValue("gain")->load())); }
 void releaseResources() override {}
 bool isBusesLayoutSupported(const BusesLayout& l) const override { auto c=l.getMainOutputChannelSet(); return (c==juce::AudioChannelSet::mono() || c==juce::AudioChannelSet::stereo()) && c==l.getMainInputChannelSet(); }
 void processBlock(juce::AudioBuffer<float>& b,juce::MidiBuffer&) override {
  juce::ScopedNoDenormals nd;
  // Measurement scaffold ONLY. No unmeasured surrogate for RBass.
  // Freq / Intensity / Original Bass are stored and automated but DSP is pending.
  smoothing.setTargetValue(juce::Decibels::decibelsToGain(state.getRawParameterValue("gain")->load()));
  for(int n=0;n<b.getNumSamples();++n) { const auto g=smoothing.getNextValue(); for(int c=0;c<b.getNumChannels();++c) b.setSample(c,n,b.getSample(c,n)*g); }
 }
 bool hasEditor() const override { return true; }
 juce::AudioProcessorEditor* createEditor() override { return new Editor(*this); }
 bool acceptsMidi() const override { return false; } bool producesMidi() const override { return false; }
 double getTailLengthSeconds() const override { return 0; }
 int getNumPrograms() override { return 1; } int getCurrentProgram() override { return 0; }
 void setCurrentProgram(int) override {} const juce::String getProgramName(int) override { return "Default"; }
 void changeProgramName(int,const juce::String&) override {}
 void getStateInformation(juce::MemoryBlock& b) override { auto xml=state.copyState().createXml(); copyXmlToBinary(*xml,b); }
 void setStateInformation(const void* d,int n) override { auto xml=getXmlFromBinary(d,n); if(xml && xml->hasTagName("RottingBassState")) state.replaceState(juce::ValueTree::fromXml(*xml)); }
 juce::AudioProcessorValueTreeState state;
private: juce::SmoothedValue<float> smoothing;
};
Editor::Editor(Processor& p) : AudioProcessorEditor(p) {
 setSize(660,380);
 juce::Slider* s[]={&freq,&intensity,&gain}; juce::Label* labels[]={&fl,&il,&gl}; const char* names[]={"Freq / Hz","Intensity / dB","Gain / dB"};
 for(int i=0;i<3;++i) { addAndMakeVisible(*s[i]); s[i]->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag); s[i]->setTextBoxStyle(juce::Slider::TextBoxBelow,false,110,24); s[i]->setColour(juce::Slider::rotarySliderFillColourId,juce::Colour(0xffb8ed65)); labels[i]->setText(names[i],juce::dontSendNotification); labels[i]->setJustificationType(juce::Justification::centred); addAndMakeVisible(*labels[i]); }
 addAndMakeVisible(original);
 original.onStateChange=[this] { original.setButtonText(original.getToggleState() ? "Original Bass IN" : "Original Bass OUT"); };
 fa=std::make_unique<SA>(p.state,"freq",freq); ia=std::make_unique<SA>(p.state,"intensity",intensity); ga=std::make_unique<SA>(p.state,"gain",gain);
 oa=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state,"originalBass",original);
}
void Editor::paint(juce::Graphics& g) {
 g.fillAll(juce::Colour(0xff101916));
 // Original vector artwork: a decaying wave in a circular bass chamber.
 g.setColour(juce::Colour(0xff334d37)); g.drawEllipse(492,14,126,126,2);
 juce::Path wave; for(int i=0;i<120;++i) { auto x=496.0f+i; auto y=77.0f+std::sin(i*0.22f)*std::exp(-i/78.0f)*42.0f; if(i==0) wave.startNewSubPath(x,y); else wave.lineTo(x,y); }
 g.setColour(juce::Colour(0xffb8ed65)); g.strokePath(wave,juce::PathStrokeType(2));
 g.setFont(38); g.drawText("RottingBass",28,22,450,50,juce::Justification::left);
 g.setFont(16); g.setColour(juce::Colour(0xff9bac9e)); g.drawText("Rottingpitstufio",30,75,420,25,juce::Justification::left);
 g.setColour(juce::Colour(0xffe6ba6b)); g.setFont(14); g.drawText("v0.1.0 / MEASUREMENT SCAFFOLD / Gain only",24,336,612,25,juce::Justification::centred);
}
void Editor::resized() { juce::Slider* s[]={&freq,&intensity,&gain}; juce::Label* l[]={&fl,&il,&gl}; for(int i=0;i<3;++i) { s[i]->setBounds(26+i*210,140,188,150); l[i]->setBounds(26+i*210,115,188,25); } original.setBounds(245,300,200,28); }
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new Processor; }
