/// @file main.cpp
/// @brief Handles the entire program, where the main program loop runs

#include <iostream>		//Debugging purposes
#include <fstream>		//Output image
#include <algorithm>	//Use of std::min when outputting image
#include <thread>		//Use of std::thread when multi-threading
#include <time.h>		//Calculate program execution time

#include <glm.hpp>

//Additional file includes
#include "Shape.h"
#include "Plane.h"
#include "Sphere.h"

//Forward declaration of functions
void InstantiateShapes(std::shared_ptr<Shape> ListOfShapes[5]);
glm::vec3 ScreenInitialisation(int &i, int &j, int &imageWidth, int &imageHeight);
void TraceRay(glm::vec3 &originOfRay, float &minT, glm::vec3 &directionOfRay, std::shared_ptr<Shape> ListOfShapes[5], int &hitShape, glm::vec3 **image, int &i, int &j);
void OutputToImage(int &imageWidth, int &imageHeight, glm::vec3 **image);
void ShootRay(int &i, int &j, int &imageWidth, int &imageHeight, glm::vec3 **image);

//0 or 1 thread
void FullScreen();

//4 threads
void TopLeft();
void TopRight();
void BottomLeft();
void BottomRight();

//16 threads (function names are coordinates for 16 element 4x4 grid, X1Y1 = Top Left, X4Y4 = Bottom Right)
void X1Y1();	//Top row
void X2Y1();
void X3Y1();
void X4Y1();

void X1Y2();	//Second row
void X2Y2();
void X3Y2();
void X4Y2();

void X1Y3();	//Third row
void X2Y3();
void X3Y3();
void X4Y3();

void X1Y4();	//Bottom row
void X2Y4();
void X3Y4();
void X4Y4();

//Input functions
void Input2();
void Input3();
void Input4();

//Global variables
std::shared_ptr<Shape> ListOfShapes[5];	//Creating an array of type shape
//Output image dimensions
int imageWidth = 800;
int imageHeight = 800;
glm::vec3 **image = new glm::vec3*[imageWidth];

void main()
{
	InstantiateShapes(ListOfShapes);		//Creating shapes

	//2D array to represent view plane
	for (int i = 0; i < imageWidth; ++i)
	{
		image[i] = new glm::vec3[imageHeight];
	}

	//Menu text
	std::cout << "Welcome to my Ray Tracer!" << std::endl;
	std::cout << "Please select the number of threads you would like to use." << std::endl << std::endl;
	std::cout << " 1. 0 Threads\n 2. 1 Thread\n 3. 4 Threads\n 4. 16 Threads\n\n 9. Exit Program!\n\n ";

	bool text = true;

	//User input
	int input;
	std::cin >> input;

	//Start execution time clock after user has selected an input
	clock_t startClock = clock();

	switch (input)
	{
		case 1:		//0 threads selected
		{
			FullScreen();
			break;
		}

		case 2:		//1 thread selected
		{
			Input2();
			break;
		}

		case 3:		//4 threads selected
		{
			Input3();
			break;
		}

		case 4:		//16 threads selected
		{
			Input4();
			break;
		}

		case 9:		//Exit program selected
		{
			text = false;
			break;
		}

		default:
		{
			std::cout << "Incorrect input!" << std::endl;
			break;
		}
	}

	if (text)
	{
		std::cout << "\n Generating image..." << std::endl;
	}

	//Output image to .ppm file
	OutputToImage(imageWidth, imageHeight, image);

	if (text)
	{
		//Calculate and print execution time of program
		printf("\n Execution Time: %.2fs\n", (double)(clock() - startClock) / CLOCKS_PER_SEC);
	}
	
	system("PAUSE");
}

void InstantiateShapes(std::shared_ptr<Shape> ListOfShapes[5])
{
	//Assigning indexes of array
	ListOfShapes[0] = std::make_shared<Plane>(glm::vec3(0, -5, 0), glm::vec3(0, 1, 0), glm::vec3(0.2f, 0.2f, 0.2f));									//Floor - Dark Grey
	ListOfShapes[1] = std::make_shared<Sphere>(glm::vec3(-10, 0, -20), 4.0f, glm::vec3(1, 0.35f, 0.35f));												//Sphere - Red
	ListOfShapes[2] = std::make_shared<Sphere>(glm::vec3(1, 0, -20), 3.0f, glm::vec3(0.35f, 1, 0.35f));													//Sphere - Green
	ListOfShapes[3] = std::make_shared<Sphere>(glm::vec3(9, 0, -20), 2.0f, glm::vec3(0.35f, 0.35f, 1));													//Sphere - Blue
	ListOfShapes[4] = std::make_shared<Sphere>(glm::vec3(14, 0, -20), 1.0f, glm::vec3(1, 1, 0.35f));													//Sphere - Yellow
}

glm::vec3 ScreenInitialisation(int &i, int &j, int &imageWidth, int &imageHeight)
{
	//Normalize pixels positions to range [0, 1] using screen dimensions, offset (+ 0.5) so ray passes through pixel centre
	float normalizePixelX = (i + 0.5f) / imageWidth;
	float normalizePixelY = (j + 0.5f) / imageHeight;

	float imageAspectRatio = imageWidth / imageHeight;	//Calculate aspect ratio of image (if not square)

	//Remap coordinates from range [0, 1] to [-1, 1], and reverse direction of Y axis
	float remapPixelX = (2 * normalizePixelX - 1) * imageAspectRatio;	//Multiply by imageAspectRatio as width is larger than height
	float remapPixelY = 1 - 2 * normalizePixelY;

	//Camera field of view (FOV) of 90
	float cameraFOVX = remapPixelX * glm::tan(glm::radians(90.0f) / 2);
	float cameraFOVY = remapPixelY * glm::tan(glm::radians(90.0f) / 2);

	//Lies on the image plane which is 1 unit from cameras origin, hence -1 in Z axis
	glm::vec3 pointCameraSpace = glm::vec3(cameraFOVX, cameraFOVY, -1);

	return pointCameraSpace;
}

void TraceRay(glm::vec3 &originOfRay, float &minT, glm::vec3 &directionOfRay, std::shared_ptr<Shape> ListOfShapes[5], int &hitShape, glm::vec3 **image, int &i, int &j)
{
	glm::vec3 p0 = originOfRay + (minT * directionOfRay);

	//Light Settings
	glm::vec3 positionOfLight = glm::vec3(20, 20, 0);	//Light position within the scene
	glm::vec3 intensityOfLight = glm::vec3(1, 1, 1);	//Brightness of the light
	//Default set to 0, declared via pointers
	glm::vec3 colourOfDiffuse = glm::vec3(0, 0, 0);
	glm::vec3 colourOfSpecular = glm::vec3(0, 0, 0);
	int shine = 0;

	//Diffuse
	glm::vec3 rayOfLight = glm::normalize(positionOfLight - p0);	//Point light in the correct direction
	glm::vec3 normal = glm::normalize(ListOfShapes[hitShape]->NormalCalculation(p0, &shine, &colourOfDiffuse, &colourOfSpecular));
	glm::vec3 diffuse = colourOfDiffuse * intensityOfLight * glm::max(0.0f, glm::dot(rayOfLight, normal));

	//Specular
	glm::vec3 reflection = glm::normalize(2 * (glm::dot(rayOfLight, normal)) * normal - rayOfLight);
	float calculateMaximum = glm::max(0.0f, glm::dot(reflection, glm::normalize(originOfRay - p0)));
	glm::vec3 specular = colourOfSpecular * intensityOfLight * glm::pow(calculateMaximum, shine);

	//Combined Lighting
	image[i][j] = diffuse + specular;	//Set pixel colour to combination of diffuse and specular lighting, phong reflection (- ambient)
}

void OutputToImage(int &imageWidth, int &imageHeight, glm::vec3 **image)
{
	//Output and save image as a .ppm
	std::ofstream ofs("./output.ppm", std::ios::out | std::ios::binary);
	ofs << "P6\n" << imageWidth << " " << imageHeight << "\n255\n";
	for (unsigned y = 0; y < imageHeight; y++)
	{
		for (unsigned x = 0; x < imageWidth; x++)
		{
			ofs << (unsigned char)(std::min((float)1, (float)image[x][y].x) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].y) * 255) <<
				(unsigned char)(std::min((float)1, (float)image[x][y].z) * 255);
		}
	}
	ofs.close();
}

void ShootRay(int &i, int &j, int &imageWidth, int &imageHeight, glm::vec3 **image)
{
	glm::vec3 pointCameraSpace = ScreenInitialisation(i, j, imageWidth, imageHeight);

	glm::vec3 originOfRay = glm::vec3(0, 0, 0);		//Origin of ray

	glm::vec3 directionOfRay = glm::normalize(pointCameraSpace - originOfRay);	//Ray shoots from (0, 0, 0) towards the camera space, normalize directionOfRay (returns direction with the magnitude of 1)

	float minT = INFINITY;	//Minimum distance
	int hitShape = -1;		//Shape that has been hit (doesn't exist at this point)
	float t0 = 0.0f;		//Point that's hit

	for (int k = 0; k < sizeof(ListOfShapes) / sizeof(ListOfShapes[0]); ++k)	//Iterate through ListOfShapes array
	{
		bool hit = ListOfShapes[k]->Intersection(&t0, originOfRay, directionOfRay);		//Store hit result of Intersection function

		//If shape is hit and the hit point is less than the minimum hit point
		if (hit && t0 < minT)
		{
			minT = t0;			//Set minimum hit point to point hit
			hitShape = k;		//Set the current shapeHit to the shape currently being iterated
		}

		//If a shape is hit
		if (hitShape != -1)
		{
			TraceRay(originOfRay, minT, directionOfRay, ListOfShapes, hitShape, image, i, j);
		}

		//Else, set the pixel colour to white (background)
		else
		{
			image[i][j] = glm::vec3(1, 1, 1);
		}
	}
}

void FullScreen()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = 0; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}

void TopLeft()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth / 2; ++i)
	{
		//Loop through pixels in Y axis
		for (int j = 0; j < imageHeight / 2; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void TopRight()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 2; i < imageWidth; ++i)
	{
		//Loop through pixels in Y axis
		for (int j = 0; j < imageHeight / 2; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void BottomLeft()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth / 2; ++i)
	{
		//Loop through pixels in Y axis
		for (int j = imageHeight / 2; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void BottomRight()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 2; i < imageWidth; ++i)
	{
		//Loop through pixels in Y axis
		for (int j = imageHeight / 2; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}

void X1Y1()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth / 4; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = 0; j < imageHeight / 4; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X2Y1()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 4; i < imageWidth / 2; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = 0; j < imageHeight / 4; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X3Y1()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 2; i < (imageWidth / 4) * 3; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = 0; j < imageHeight / 4; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X4Y1()
{
	//Loop through pixels in X axis
	for (int i = (imageWidth / 4) * 3; i < imageWidth; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = 0; j < imageHeight / 4; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}

void X1Y2()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth / 4; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 4; j < imageHeight / 2; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X2Y2()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 4; i < imageWidth / 2; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 4; j < imageHeight / 2; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X3Y2()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 2; i < (imageWidth / 4) * 3; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 4; j < imageHeight / 2; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X4Y2()
{
	//Loop through pixels in X axis
	for (int i = (imageWidth / 4) * 3; i < imageWidth; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 4; j < imageHeight / 2; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}

void X1Y3()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth / 4; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 2; j < (imageHeight / 4) * 3; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X2Y3()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 4; i < imageWidth / 2; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 2; j < (imageHeight / 4) * 3; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X3Y3()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 2; i < (imageWidth / 4) * 3; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 2; j < (imageHeight / 4) * 3; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X4Y3()
{
	//Loop through pixels in X axis
	for (int i = (imageWidth / 4) * 3; i < imageWidth; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = imageHeight / 2; j < (imageHeight / 4) * 3; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}

void X1Y4()
{
	//Loop through pixels in X axis
	for (int i = 0; i < imageWidth / 4; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = (imageHeight / 4) * 3; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X2Y4()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 4; i < imageWidth / 2; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = (imageHeight / 4) * 3; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X3Y4()
{
	//Loop through pixels in X axis
	for (int i = imageWidth / 2; i < (imageWidth / 4) * 3; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = (imageHeight / 4) * 3; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}
void X4Y4()
{
	//Loop through pixels in X axis
	for (int i = (imageWidth / 4) * 3; i < imageWidth; ++i)
	{
		//Loop through all pixels in Y axis
		for (int j = (imageHeight / 4) * 3; j < imageHeight; ++j)
		{
			ShootRay(i, j, imageWidth, imageHeight, image);
		}
	}
}

void Input2()
{
	std::thread thread1(FullScreen);
	thread1.join();
}

void Input3()
{
	std::thread thread1(TopLeft);
	std::thread thread2(TopRight);
	std::thread thread3(BottomLeft);
	std::thread thread4(BottomRight);

	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
}

void Input4()
{
	std::thread thread1(X1Y1);
	std::thread thread2(X2Y1);
	std::thread thread3(X3Y1);
	std::thread thread4(X4Y1);

	std::thread thread5(X1Y2);
	std::thread thread6(X2Y2);
	std::thread thread7(X3Y2);
	std::thread thread8(X4Y2);

	std::thread thread9(X1Y3);
	std::thread thread10(X2Y3);
	std::thread thread11(X3Y3);
	std::thread thread12(X4Y3);

	std::thread thread13(X1Y4);
	std::thread thread14(X2Y4);
	std::thread thread15(X3Y4);
	std::thread thread16(X4Y4);

	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();

	thread5.join();
	thread6.join();
	thread7.join();
	thread8.join();

	thread9.join();
	thread10.join();
	thread11.join();
	thread12.join();

	thread13.join();
	thread14.join();
	thread15.join();
	thread16.join();
}