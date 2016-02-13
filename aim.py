#!/usr/bin/env python3

import math

height = 2.464
#height = 1
init_v = 8.166

print("Angle\tDist 1\tDist 2")
for i in range(53, 81):
    angle = i / 180 * math.pi
    a = -9.8*.5
    b = init_v * math.sin(angle)
    c = -height
    if b**2 - 4*a*c < 0:
        #print("No solution for angle {}\ta={:.4}\tb={:.4}\tc={:.4}\tsqrt={:.4}".format(i, a, b, c, b**2 - 4*a*c))
        continue
    possibles = ((-b + math.sqrt(b**2 - 4*a*c))/(2*a),
                 (-b - math.sqrt(b**2 - 4*a*c))/(2*a))
    dists = list(map(lambda t: init_v*math.cos(angle)*t, possibles))
    print("{}\t{:.2}\t{:.2}".format(i, dists[0], dists[1]))
