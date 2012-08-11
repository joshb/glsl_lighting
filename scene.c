/*
 * Copyright (C) 2010 Josh A. Beam
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef _WIN32
	#include <windows.h>
	#include <wingdi.h>
	#define GLUT_DISABLE_ATEXIT_HACK
#else
	#include <sys/time.h>
#endif /* _WIN32 */
#include "opengl.h"

/* shader functions defined in shader.c */
extern void shaderAttachFromFile(GLuint, GLenum, const char *);

static GLuint g_program;
static GLuint g_programCameraPositionLocation;
static GLuint g_programLightPositionLocation;
static GLuint g_programLightColorLocation;

static GLuint g_cylinderBufferId;
static unsigned int g_cylinderNumVertices;

static float g_cameraPosition[3];

#define NUM_LIGHTS 3
static float g_lightPosition[NUM_LIGHTS * 3];
static float g_lightColor[NUM_LIGHTS * 3];
static float g_lightRotation;

static void
createCylinder(unsigned int divisions)
{
	const int floatsPerVertex = 6;
	unsigned int i, size;
	float *v;

	g_cylinderNumVertices = (divisions + 1) * 2;
	size = floatsPerVertex * g_cylinderNumVertices;

	/* generate vertex data */
	v = malloc(sizeof(float) * size);
	for(i = 0; i <= divisions; ++i) {
		float r = ((M_PI * 2.0f) / (float)divisions) * (float)i;
		unsigned int index1 = i * 2 * floatsPerVertex;
		unsigned int index2 = index1 + floatsPerVertex;

		/* vertex positions */
		v[index1 + 0] = cosf(r);
		v[index1 + 1] = 1.0f;
		v[index1 + 2] = -sinf(r);
		v[index2 + 0] = cosf(r);
		v[index2 + 1] = -1.0f;
		v[index2 + 2] = -sinf(r);

		/* normals */
		v[index1 + 3] = cosf(r);
		v[index1 + 4] = 0.0f;
		v[index1 + 5] = -sinf(r);
		v[index2 + 3] = v[index1 + 3];
		v[index2 + 4] = v[index1 + 4];
		v[index2 + 5] = v[index1 + 5];
	}

	/* create vertex buffer */
	glGenBuffers(1, &g_cylinderBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, g_cylinderBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, v, GL_STATIC_DRAW);
	free(v);

	/* enable arrays */
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	/* set pointers */
	glVertexPointer(3, GL_FLOAT, sizeof(float) * floatsPerVertex, 0);
	glNormalPointer(GL_FLOAT, sizeof(float) * floatsPerVertex, (const GLvoid *)(sizeof(float) * 3));
}

void sceneCycle(void);

void
sceneInit(void)
{
	GLint result;

	/* create program object and attach shaders */
	g_program = glCreateProgram();
	shaderAttachFromFile(g_program, GL_VERTEX_SHADER, "data/shader.vp");
	shaderAttachFromFile(g_program, GL_FRAGMENT_SHADER, "data/shader.fp");

	/* link the program and make sure that there were no errors */
	glLinkProgram(g_program);
	glGetProgramiv(g_program, GL_LINK_STATUS, &result);
	if(result == GL_FALSE) {
		GLint length;
		char *log;

		/* get the program info log */
		glGetProgramiv(g_program, GL_INFO_LOG_LENGTH, &length);
		log = malloc(length);
		glGetProgramInfoLog(g_program, length, &result, log);

		/* print an error message and the info log */
		fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
		free(log);

		/* delete the program */
		glDeleteProgram(g_program);
		g_program = 0;
	}

	/* get uniform locations */
	g_programCameraPositionLocation = glGetUniformLocation(g_program, "cameraPosition");
	g_programLightPositionLocation = glGetUniformLocation(g_program, "lightPosition");
	g_programLightColorLocation = glGetUniformLocation(g_program, "lightColor");

	/* set up red/green/blue lights */
	g_lightColor[0] = 1.0f; g_lightColor[1] = 0.0f; g_lightColor[2] = 0.0f;
	g_lightColor[3] = 0.0f; g_lightColor[4] = 1.0f; g_lightColor[5] = 0.0f;
	g_lightColor[6] = 0.0f; g_lightColor[7] = 0.0f; g_lightColor[8] = 1.0f;

	/* create cylinder */
	createCylinder(36);

	/* do the first cycle to initialize positions */
	g_lightRotation = 0.0f;
	sceneCycle();

	/* setup camera */
	g_cameraPosition[0] = 0.0f;
	g_cameraPosition[1] = 0.0f;
	g_cameraPosition[2] = 4.0f;
	glLoadIdentity();
	glTranslatef(-g_cameraPosition[0], -g_cameraPosition[1], -g_cameraPosition[2]);
}

void
sceneRender(void)
{
	int i;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* enable program and set uniform variables */
	glUseProgram(g_program);
	glUniform3fv(g_programCameraPositionLocation, 1, g_cameraPosition);
	glUniform3fv(g_programLightPositionLocation, NUM_LIGHTS, g_lightPosition);
	glUniform3fv(g_programLightColorLocation, NUM_LIGHTS, g_lightColor);

	/* render the cylinder */
	glDrawArrays(GL_TRIANGLE_STRIP, 0, g_cylinderNumVertices);

	/* disable program */
	glUseProgram(0);

	/* render each light */
	for(i = 0; i < NUM_LIGHTS; ++i) {
		/* render sphere with the light's color/position */
		glPushMatrix();
		glTranslatef(g_lightPosition[i * 3 + 0], g_lightPosition[i * 3 + 1], g_lightPosition[i * 3 + 2]);
		glColor3fv(g_lightColor + (i * 3));
		glutSolidSphere(0.04, 36, 36);
		glPopMatrix();
	}

	glutSwapBuffers();
}

static unsigned int
getTicks(void)
{
#ifdef _WIN32
	return GetTickCount();
#else
	struct timeval t;

	gettimeofday(&t, NULL);

	return (t.tv_sec * 1000) + (t.tv_usec / 1000);
#endif /* _WIN32 */
}

void
sceneCycle(void)
{
	static unsigned int prevTicks = 0;
	unsigned int ticks;
	float secondsElapsed;
	int i;

	/* calculate the number of seconds that have
	 * passed since the last call to this function */
	if(prevTicks == 0)
		prevTicks = getTicks();
	ticks = getTicks();
	secondsElapsed = (float)(ticks - prevTicks) / 1000.0f;
	prevTicks = ticks;

	/* update the light positions */
	g_lightRotation += (M_PI / 4.0f) * secondsElapsed;
	for(i = 0; i < NUM_LIGHTS; ++i) {
		const float radius = 1.75f;
		float r = (((M_PI * 2.0f) / (float)NUM_LIGHTS) * (float)i) + g_lightRotation;

		g_lightPosition[i * 3 + 0] = cosf(r) * radius;
		g_lightPosition[i * 3 + 1] = cosf(r) * sinf(r);
		g_lightPosition[i * 3 + 2] = sinf(r) * radius;
	}

	glutPostRedisplay();
}
