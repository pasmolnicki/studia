from ctypes import CDLL, c_int, Structure
import os

_LIB_PATH = os.path.join(os.getcwd(), 'libnums.so')
_lib = CDLL(_LIB_PATH)

class DiofantResult(Structure):
    _fields_ = [('x', c_int), ('y', c_int)]

_lib.gcd.argtypes = (c_int, c_int)
_lib.gcd.restype = c_int

_lib.min_divider.argtypes = (c_int,)
_lib.min_divider.restype = c_int

_lib.totient.argtypes = (c_int,)
_lib.totient.restype = c_int

_lib.diofant.argtypes = (c_int, c_int, c_int)
_lib.diofant.restype = DiofantResult

def gcd(a, b):
    return _lib.gcd(a, b)

def min_divider(n):
    return _lib.min_divider(n)

def totient(n):
    return _lib.totient(n)

def diofant(a, b, c):
    r = _lib.diofant(a, b, c)
    return (r.x, r.y)
