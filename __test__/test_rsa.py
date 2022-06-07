from random import getrandbits
from Crypto.Util.number import getPrime

e = 65537
p = getPrime(1024)
q = getPrime(1024)
n = p*q
d = pow(e, -1, (p-1)*(q-1))

m = getrandbits(2048)
print(f'[ i ] Input plaintext: {m}')

c = pow(m, e, n)
print(f'[ i ] Output ciphertext: {c}')
print(f'[ i ] Decrypted: {pow(c, d, n)}')