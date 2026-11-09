"""Strict null: confirmed integer delay only. No fitted gain/EQ/phase."""
import argparse,json
from pathlib import Path
import numpy as np
from scipy.io import wavfile

def read(p):
 sr,x=wavfile.read(p)
 if x.dtype!=np.float32: raise ValueError(f'{p}: expected FLOAT32 WAV')
 if x.ndim==1:x=x[:,None]
 if not np.isfinite(x).all():raise ValueError(f'{p}: nonfinite samples')
 return sr,x.astype(np.float64)
def db(x):return float(20*np.log10(x)) if x>0 else None
def rms(x):return float(np.sqrt(np.mean(x*x)))
def compare(ref,test,sr,delay,entry):
 if ref.shape!=test.shape:raise ValueError('shape mismatch')
 # Positive delay means test lags reference. Compare overlapping samples only.
 if abs(delay)>=len(ref):raise ValueError('delay exceeds file')
 r=ref[:len(ref)-delay] if delay>0 else ref[-delay:] if delay<0 else ref
 s=test[delay:] if delay>0 else test[:len(test)+delay] if delay<0 else test
 e=s-r; rr=rms(r); er=rms(e)
 out=dict(exact_zero=bool(np.array_equal(r,s)),residual_peak_dbfs=db(np.max(np.abs(e))),residual_rms_dbfs=db(er),reference_rms_dbfs=db(rr),residual_relative_db=db(er/rr) if rr else None,pass_60dB=(er<=rr*0.001) if rr else None,compared_frames=len(r),confirmed_delay=delay,harmonics=[])
 if entry.get('kind')=='sine':
  lo,hi=entry['steady'];lo=max(lo,-delay);hi=min(hi,len(test)-delay)
  tt=np.arange(lo,hi)/sr
  for f in entry['frequencies']:
   for h in range(1,min(16,int(sr/2/f))+1):
    basis=np.exp(-2j*np.pi*h*f*tt)
    for c in range(ref.shape[1]):
     ar=2*np.mean(ref[lo:hi,c]*basis);at=2*np.mean(test[lo+delay:hi+delay,c]*basis)
     out['harmonics'].append(dict(h=h,hz=f*h,channel=c,reference_dbfs=db(abs(ar)),candidate_dbfs=db(abs(at)),delta_db=db(abs(at)/abs(ar)) if abs(ar)>1e-10 else None,phase_delta_deg=float(np.angle(at*np.conj(ar),deg=True)) if abs(ar)>1e-10 else None))
 return out
if __name__=='__main__':
 p=argparse.ArgumentParser();p.add_argument('manifest');p.add_argument('reference');p.add_argument('candidate');p.add_argument('--split',choices=['fit','validation'],default='validation');p.add_argument('--confirmed-delay',type=int,required=True);p.add_argument('--delay-evidence',required=True);p.add_argument('--output',default='report.json');a=p.parse_args()
 entries=json.loads(Path(a.manifest).read_text());results=[]
 for en in entries:
  if en['split']!=a.split:continue
  name=Path(en['file']).name
  try:
   sr,r=read(Path(a.reference)/name);st,s=read(Path(a.candidate)/name)
   if sr!=st or sr!=en['rate'] or len(r)!=en['frames'] or r.shape[1]!=en['channels']:raise ValueError('rate/length mismatch')
   results.append(dict(file=name,**compare(r,s,sr,a.confirmed_delay,en)))
  except (ValueError,FileNotFoundError) as e:results.append(dict(file=name,error=str(e)))
 valid=[r for r in results if r.get('residual_relative_db') is not None]
 valid.sort(key=lambda r:r['residual_relative_db'],reverse=True)
 Path(a.output).write_text(json.dumps(dict(delay_evidence=a.delay_evidence,minus_infinity='null dB denotes zero amplitude',results=results,worst_files=[r['file'] for r in valid],complete=not any('error' in r for r in results),all_pass=bool(results) and not any('error' in r for r in results) and any(r.get('reference_rms_dbfs') is not None for r in results) and all(r.get('pass_60dB') is True for r in results if r.get('reference_rms_dbfs') is not None)),indent=2,allow_nan=False))
