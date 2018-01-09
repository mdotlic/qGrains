#include "triangle.h"
typedef REAL *vertex;
int triunsuitable(vertex triorg, vertex tridest, vertex triapex, REAL area)
{
  REAL dxoa, dxda, dxod;
  REAL dyoa, dyda, dyod;
  REAL oalen, dalen, odlen;
  REAL maxlen;

  dxoa = triorg[0] - triapex[0];
  dyoa = triorg[1] - triapex[1];
  dxda = tridest[0] - triapex[0];
  dyda = tridest[1] - triapex[1];
  dxod = triorg[0] - tridest[0];
  dyod = triorg[1] - tridest[1];
  /* Find the squares of the lengths of the triangle's three edges. */
  oalen = dxoa * dxoa + dyoa * dyoa;
  dalen = dxda * dxda + dyda * dyda;
  odlen = dxod * dxod + dyod * dyod;
  /* Find the square of the length of the longest edge. */
  maxlen = (dalen > oalen) ? dalen : oalen;
  maxlen = (odlen > maxlen) ? odlen : maxlen;

  if (maxlen > 0.05 * (triorg[0] * triorg[0] + triorg[1] * triorg[1]) + 0.02) {
    return 1;
  } else {
    return 0;
  }
}

