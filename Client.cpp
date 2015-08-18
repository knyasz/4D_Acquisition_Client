/*
 * Client.cpp
 *
 *  Created on: Aug 18, 2015
 *      Author: alexandalex
 */


#include "MyClientDepthRunner.h"
#include "MyClientRGBRunner.h"

int main(int argc, char **argv) {
//	MyClientRunner_I::SetDevice();
	MyClientRGBRunner RGB;
	MyClientDepthRunner Depth;
	RGB.Run();
	Depth.Run();


	return 0;
}

