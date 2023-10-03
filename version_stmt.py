import sys

s = sys.argv[-1]
v = s.removeprefix('v')
if s == v: exit(1)

print(f"__version__ = '{v}'")
