"""Deterministic 32-bit float WAV stimuli. Requires numpy/scipy."""
import argparse, json, hashlib
from pathlib import Path
import numpy as np
from scipy.io import wavfile
p=argparse.ArgumentParser(); p.add_argument('--root',default='measurements'); p.add_argument('--rate',type=int,default=48000); a=p.parse_args()
root=Path(a.root); sr=a.rate; N=sr*6; t=np.arange(sr*4)/sr; manifest=[]
def save(name,x,split,kind,frequencies=[],level=None):
 y=np.zeros((N,2),dtype=np.float32); x=np.asarray(x)
 if x.ndim==1:x=np.column_stack([x,x])
 y[sr:sr+len(x)]=x
 path=root/'inputs'/split/(name+'.wav');path.parent.mkdir(parents=True,exist_ok=True);wavfile.write(path,sr,y)
 manifest.append(dict(file=str(path.relative_to(root)),split=split,kind=kind,frequencies=frequencies,level_dbfs=level,frames=N,rate=sr,channels=2,active=[sr,5*sr],steady=[2*sr,4*sr],sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
for split,fs,levels in [('fit',[25,40,55,63,66,69,80,100,132,200],[-36,-18,-6]),('validation',[31,61,73,117],[-27,-12])]:
 for f in fs:
  for db in levels:save(f'sine_{f}Hz_{db}dB',10**(db/20)*np.sin(2*np.pi*f*t),split,'sine',[f],db)
 for db in levels:
  save(f'two_43_71_{db}dB',10**(db/20)*0.5*(np.sin(2*np.pi*43*t)+np.sin(2*np.pi*71*t)),split,'two',[43,71],db)
  f=52 if split=='fit' else 59
  save(f'kick_{f}_{db}dB',10**(db/20)*(1-np.exp(-t/0.001))*np.exp(-t/0.21)*np.sin(2*np.pi*(f*t+8*0.035*(1-np.exp(-t/0.035)))),split,'kick',[f],db)
x=np.zeros_like(t); x[0]=0.5;x[sr]=-0.5;x[2*sr:2*sr+8]=0.25; save('impulses',x,'fit','impulse')
for mode in ['L','R','LR','anti']:
 s=0.25*np.sin(2*np.pi*66*t);z=np.zeros_like(s)
 save('stereo_'+mode,np.column_stack([s if mode!='R' else z, z if mode=='L' else -s if mode=='anti' else s]),'fit','stereo',[66])
save('silence',np.zeros_like(t),'fit','silence')
root.mkdir(parents=True,exist_ok=True); (root/'manifest.json').write_text(json.dumps(manifest,indent=2))
print(f'{len(manifest)} WAV files, {sr} Hz, FLOAT32, stereo, 6 seconds each')
