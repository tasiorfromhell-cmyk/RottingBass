import sys, unittest
from pathlib import Path
import numpy as np
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'tools'))
from analyze import compare
class NullTests(unittest.TestCase):
 def setUp(self):
  self.sr=48000; self.x=np.sin(2*np.pi*66*np.arange(48000)/48000)[:,None]*0.2
 def test_identity(self):
  r=compare(self.x,self.x,self.sr,0,{})
  self.assertTrue(r['exact_zero']);self.assertTrue(r['pass_60dB'])
 def test_gain_error_detected(self):
  r=compare(self.x,self.x*1.01,self.sr,0,{})
  self.assertAlmostEqual(r['residual_relative_db'],-40,places=9);self.assertFalse(r['pass_60dB'])
 def test_delay(self):
  y=np.concatenate([np.zeros((13,1)),self.x[:-13]])
  self.assertTrue(compare(self.x,y,self.sr,13,{})['exact_zero'])
  self.assertFalse(compare(self.x,y,self.sr,0,{})['pass_60dB'])
 def test_silence(self):
  r=compare(self.x*0,self.x*0,self.sr,0,{})
  self.assertIsNone(r['pass_60dB']);self.assertIsNone(r['residual_relative_db'])
 def test_shape(self):
  with self.assertRaises(ValueError):compare(self.x,self.x[:-1],self.sr,0,{})
if __name__=='__main__':unittest.main()
