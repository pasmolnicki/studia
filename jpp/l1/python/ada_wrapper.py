from ctypes import CDLL, c_int, Structure
import os

_LIB_PATH = os.path.join(os.getcwd(), 'libada_nums.so')
_lib = CDLL(_LIB_PATH)

class DiofantResult(Structure):
    _fields_ = [('x', c_int), ('y', c_int)]

_lib.ada_gcd.argtypes = (c_int, c_int)
_lib.ada_gcd.restype = c_int

_lib.ada_min_divider.argtypes = (c_int,)
_lib.ada_min_divider.restype = c_int

_lib.ada_totient.argtypes = (c_int,)
_lib.ada_totient.restype = c_int

_lib.ada_diofant.argtypes = (c_int, c_int, c_int)
_lib.ada_diofant.restype = DiofantResult

def gcd(a, b):
    return _lib.ada_gcd(a, b)

def min_divider(n):
    return _lib.ada_min_divider(n)

def totient(n):
    return _lib.ada_totient(n)

def diofant(a, b, c):
    r = _lib.ada_diofant(a, b, c)
    return (r.x, r.y)
