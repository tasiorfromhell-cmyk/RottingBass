"""RBass IN minus OUT, no gain compensation. Preserve full timeline."""
import argparse
from pathlib import Path
from analyze import read
from scipy.io import wavfile
p=argparse.ArgumentParser();p.add_argument('in_dir');p.add_argument('out_dir');p.add_argument('destination');a=p.parse_args()
d=Path(a.destination);d.mkdir(parents=True,exist_ok=True)
files=sorted(Path(a.in_dir).glob('*.wav'))
if not files:raise ValueError('No WAV files in IN directory')
for f in files:
 sr,x=read(f);st,y=read(Path(a.out_dir)/f.name)
 if sr!=st or x.shape!=y.shape:raise ValueError(f.name)
 wavfile.write(d/f.name,sr,(x-y).astype('float32'))
