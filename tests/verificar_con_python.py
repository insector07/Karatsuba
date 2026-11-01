"""Verificacion INDEPENDIENTE: compara el producto de bin/calc contra los enteros de Python.
Uso: make bin/calc && python3 tests/verificar_con_python.py"""
import random, subprocess, sys
sys.set_int_max_str_digits(0)
random.seed(7)
casos = [(1, 1), (3, 100), (500, 500), (1000, 1000), (4096, 4096), (777, 1234)]
for na, nb in casos:
    a = random.randrange(10 ** (na - 1), 10 ** na) if na > 1 else random.randrange(1, 10)
    b = random.randrange(10 ** (nb - 1), 10 ** nb) if nb > 1 else random.randrange(1, 10)
    out = subprocess.run(["./bin/calc", str(a), str(b)], capture_output=True, text=True, check=True).stdout.split()
    ok = int(out[0]) == a * b and int(out[1]) == a * b
    print(("ok   " if ok else "FALLO"), f"{na} x {nb} digitos")
    assert ok
print("Verificacion con Python: todo correcto")
