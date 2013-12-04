// NDIAPI_Test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NDIAPI.h"
#include <Windows.h>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
    int errnum = Probe();
	if (errnum != 0)
	{
		printf("Failed to connect\n");
		return -1;
	}
	
	std::cout << "Probing found device on: " << GetConnectedDevicePortName() << std::endl;
	
	Open(NULL, false);

	std::cout << "Device Name: " << GetConnectedDeviceName() << std::endl;
	std::cout << "Device Version: " << GetConnectedDeviceVersion() << std::endl;
	std::cout << "Device Serial: " << GetConnectedDeviceSerialNumber() << std::endl;

	std::cout << "Loading tool files..." << std::endl;
	LoadToolFromFile(0, "c:/users/mfoster/Downloads/8700338.rom");
	LoadToolFromFile(1, "c:/users/mfoster/Downloads/8700339.rom");

	std::cout << "Starting tracking..." << std::endl;
	StartTracking(true);

	std::cout << "Tracking..." << std::endl;
	QuaternionTransformStruct* toolTransform[2];

	while(1)
	{
		Sleep(250);

		if (UpdateTransforms())
		{
			toolTransform[0] = GetQuaternionTransform(0);
			if (toolTransform[0] != NULL && toolTransform[0]->status==0)
			{
				printf("q0: %lf\nq1: %lf\nq2: %lf\nq3: %lf\nx: %lf\ny: %lf\nz: %lf\n\n", toolTransform[0]->q0,
																					     toolTransform[0]->q1,
																						 toolTransform[0]->q2,
																						 toolTransform[0]->q3,
																						 toolTransform[0]->x,
																						 toolTransform[0]->y,
																						 toolTransform[0]->z);
			}

			toolTransform[1] = GetQuaternionTransform(1);

			delete toolTransform[0];
			delete toolTransform[1];
		}
	}

	printf("Stoping tracking...\n");
	StopTracking();

	return 0;
}

