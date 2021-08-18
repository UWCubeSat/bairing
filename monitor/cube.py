import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
import numpy as np

vertices = (
  (1, -1, -1),
  (1, 1, -1),
  (-1, 1, -1),
  (-1, -1, -1),
  (1, -1, 1),
  (1, 1, 1),
  (-1, -1, 1),
  (-1, 1, 1),
  (0, 0, 1),
  (0, 0, 2)
)
edges = (
  (0,1),
  (0,3),
  (0,4),
  (2,1),
  (2,3),
  (2,7),
  (6,3),
  (6,4),
  (6,7),
  (5,1),
  (5,4),
  (5,7),
  (8,9)
)

def cube():
  glBegin(GL_LINES)
  for edge in edges:
    for vertex in edge:
      glVertex3fv(vertices[vertex])
  glEnd()
  
def q_to_mat4(q):
  w, x, y, z = q
  return np.array(
      [[1 - 2*y*y - 2*z*z, 2*x*y - 2*z*w, 2*x*z + 2*y*w, 0],
      [2*x*y + 2*z*w, 1 - 2*x*x - 2*z*z, 2*y*z - 2*x*w, 0],
      [2*x*z - 2*y*w, 2*y*z + 2*x*w, 1 - 2*x*x - 2*y*y, 0],
      [0, 0, 0, 1] ],'f')

class CubeRenderer:
  def __init__(self):
    pygame.init()
    self.display = (1280, 768)
    pygame.display.set_mode(self.display, DOUBLEBUF | OPENGL)
  
  def render(self, q):
    for event in pygame.event.get():
      if event.type == pygame.QUIT:
        pygame.quit()
        quit()
    glLoadIdentity();
    gluPerspective(45, (self.display[0]/self.display[1]), 0.1, 50.0)
    glTranslatef(0.0,0.0, -5)
    glRotatef(180, 1, 0, 0)
    glMultMatrixf(q_to_mat4(q))
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    cube()
    pygame.display.flip()

def main():  
  renderer = CubeRenderer()
  
  while True:
    renderer.render([0.7071, 0.0, 0.7071, 0.0])
    pygame.time.wait(10)
  
if __name__ == '__main__':
    main()