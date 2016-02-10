#include <stdlib.h>

typedef unsigned char GLubyte;
typedef unsigned int GLuint;
typedef unsigned char GLboolean;
#define GL_TRUE 1
#define GL_FALSE 0

static GLboolean _glewStrSame (const GLubyte* a, const GLubyte* b, GLuint n)
{
  GLuint i = 0;
  if (a == NULL || b == NULL)
    return (a == NULL && b == NULL && n == 0) ? GL_TRUE : GL_FALSE;
  while (i < n && a[i] != '\0' && b[i] != '\0' && a[i] == b[i])
	  i++;
  return i == n ? GL_TRUE : GL_FALSE;
}

int main()
{
	return 0;
}