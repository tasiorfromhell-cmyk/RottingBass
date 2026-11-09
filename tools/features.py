"""Raw observations for identification; delay estimate is NOT confirmation."""
import argparse,json
from pathlib import Path
import numpy as np
from scipy.signal import correlate,correlation_lags
from analyze import read,rms,db
p=argparse.ArgumentParser();p.add_argument('manifest');p.add_argument('inputs');p.add_argument('renders');p.add_argument('--output',default='features.json');a=p.parse_args()
rows=[]
for en in json.loads(Path(a.manifest).read_text()):
 if en['split']!='fit':continue
 name=Path(en['file']).name
 sr,x=read(Path(a.inputs)/en['file']);st,y=read(Path(a.renders)/name)
 if sr!=st or x.shape!=y.shape:raise ValueError(name+': mismatch')
 row=dict(file=name,kind=en['kind'],channels=[])
 lo,hi=en['steady'];tt=np.arange(lo,hi)/sr
 fs=en['frequencies'];freqs=set()
 if en['kind'] in ['sine','stereo']:freqs={f*h for f in fs for h in range(1,17) if f*h<sr/2}
 if en['kind']=='two':freqs={abs(m*fs[0]+n*fs[1]) for m in range(-5,6) for n in range(-5,6) if 0<abs(m)+abs(n)<=5 and 0<abs(m*fs[0]+n*fs[1])<sr/2}
 for c in range(x.shape[1]):
  lines=[]
  for f in sorted(freqs):
   basis=np.exp(-2j*np.pi*f*tt);xx=2*np.mean(x[lo:hi,c]*basis);yy=2*np.mean(y[lo:hi,c]*basis)
   lines.append(dict(hz=f,input_dbfs=db(abs(xx)),output_dbfs=db(abs(yy)),input_phase_deg=float(np.angle(xx,deg=True)),output_phase_deg=float(np.angle(yy,deg=True))))
  envelope=[dict(time_s=i/sr,input_rms=rms(x[i:i+max(1,sr//100),c]),output_rms=rms(y[i:i+max(1,sr//100),c])) for i in range(0,len(x),max(1,sr//100))]
  hypothesis=None
  if rms(x[:,c])>0 and rms(y[:,c])>0:
   corr=correlate(y[:,c],x[:,c],method='fft');lags=correlation_lags(len(y),len(x)); mask=abs(lags)<=sr//10
   hypothesis=int(lags[mask][np.argmax(abs(corr[mask]))])
  row['channels'].append(dict(channel=c,input_rms_dbfs=db(rms(x[:,c])),output_rms_dbfs=db(rms(y[:,c])),output_peak_dbfs=db(np.max(abs(y[:,c]))),dc=float(np.mean(y[:,c])),delay_hypothesis_samples=hypothesis,lines=lines,envelope_10ms=envelope))
 rows.append(row)
Path(a.output).write_text(json.dumps(dict(warning='Correlation delay is a hypothesis. Steady spectra use 2-4 s; transient phases need separate interpretation.',results=rows),indent=2,allow_nan=False))
