"""Blind pairs with common headroom and RMS matching, separate from strict null."""
import argparse,json,secrets
from pathlib import Path
from analyze import read,rms
from scipy.io import wavfile
import numpy as np
p=argparse.ArgumentParser();p.add_argument('reference');p.add_argument('candidate');p.add_argument('destination');a=p.parse_args()
sr,r=read(a.reference);st,s=read(a.candidate)
if sr!=st or r.shape!=s.shape:raise ValueError('rate/shape mismatch')
if min(rms(r),rms(s))==0:raise ValueError('silent input')
s=s*rms(r)/rms(s);scale=min(1,0.891250938/max(np.max(abs(r)),np.max(abs(s))))
r*=scale;s*=scale
flip=secrets.randbelow(2);pair=[r,s] if not flip else [s,r];xi=secrets.randbelow(2)
d=Path(a.destination);d.mkdir(parents=True,exist_ok=True)
for n,x in zip(['A','B','X'],[pair[0],pair[1],pair[xi]]):wavfile.write(d/(n+'.wav'),sr,x.astype('float32'))
(d/'KEY_PRIVATE.json').write_text(json.dumps(dict(A='candidate' if flip else 'reference',X='AB'[xi],matching='whole-file RMS; common peak headroom -1 dBFS')))
