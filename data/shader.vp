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

const int NUM_LIGHTS = 3;

uniform vec3 cameraPosition;
uniform vec3 lightPosition[NUM_LIGHTS];

varying vec3 fragmentNormal;
varying vec3 cameraVector;
varying vec3 lightVector[NUM_LIGHTS];

void
main()
{
	// set the normal for the fragment shader and
	// the vector from the vertex to the camera
	fragmentNormal = gl_Normal;
	cameraVector = cameraPosition - gl_Vertex.xyz;

	// set the vectors from the vertex to each light
	for(int i = 0; i < NUM_LIGHTS; ++i)
		lightVector[i] = lightPosition[i] - gl_Vertex.xyz;

	// output the transformed vertex
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
