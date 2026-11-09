RottingBass now generates bass harmonics with original DSP. All four controls affect audio: Freq selects the low band, Intensity adjusts generated harmonics, Original Bass IN/OUT retains/removes the extracted low band, and Gain controls output. Includes 4x oversampling, smoothed automation, independent mono/stereo processing and state recall.

The attached ZIP contains the complete Windows x64 VST3 bundle built with MSVC and static C++ runtime. CI must pass processor tests and dynamic VST3 loading/audio tests on Windows before publishing. Test logs are included. Product/manufacturer and VST3 class IDs are checked.

REAPER-specific verification has not been performed. No RBass references have been supplied: this is an original approximation, not a calibrated or sample-identical recreation. No RBass null results or -60 dB residual claim.

Install: extract and copy the complete RottingBass.vst3 directory to C:\Program Files\Common Files\VST3, then rescan in REAPER x64. Keep previous ZIPs; v0.1.0 is preserved in archives/v0.1.0.
