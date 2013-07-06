#include <Windows.h>
#include <gl\glut.h>

#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <stack>
#include <map>
#include "GridGenerator2D.h"

using namespace std;

const int WINDOW_WIDTH = 1024;
const int WINDOW_HEIGHT = 800;

// Parameters to determine cell types
const double ELEVATION_CHECK_HIGHMOUNTAINS = 0.15;
const double ELEVATION_CHECK_LOWMOUNTAINS = 0.30;
const double ELEVATION_CHECK_FLATLAND = 0.70;
const double ELEVATION_CHECK_SHORE = 0.75;
const double ELEVATION_CHECK_SEA = 0.85;
const double ELEVATION_CHECK_OCEAN = 1.00;

const double TEMPERATURE_CHECK_HOT = 0.40;
const double TEMPERATURE_CHECK_WARM = 0.75;
const double TEMPERATURE_CHECK_COLD = 1.00;

const double VEGETATION_CHECK_LUSH = 0.40;
const double VEGETATION_CHECK_NORMAL = 0.75;
const double VEGETATION_CHECK_ROUGH = 1.00;

// Define types of outputs for special preferences and the types of cells we can have
enum OutputType { EARTHLIKE, GRAYSCALE_ELEVATION, GRAYSCALE_TEMPERATURE, GRAYSCALE_VEGETATION };
const int NUM_OUTPUT_TYPES = 4;
enum MapCellType { UNKNOWN, 
				PLAINS_DEEP_MOUNTAIN, PLAINS_LIGHT_MOUNTAIN,
				DESERT_DEEP_MOUNTAIN, DESERT_LIGHT_MOUNTAIN,
				SNOW_DEEP_MOUNTAIN, SNOW_LIGHT_MOUNTAIN, 
				SHORE, SEA, OCEAN, 
				PLAINS, DESERT, SNOW, 
				PLAINS_FOREST, DESERT_FOREST, SNOW_FOREST };

class MapGenerator
{
private:

	int width;
	int height;
	int size;
	
	GridGenerator2D gen;
	OutputType output;
	
	map<MapCellType, string> cellCodes;
	map<string, string> outputCodes;

	double elevationMin;
	double elevationMax;
	vector< vector<double> > elevationMap;
	
	double temperatureMin;
	double temperatureMax;
	vector< vector<double> > temperatureMap;

	double vegetationMin;
	double vegetationMax;
	vector< vector<double> > vegetationMap;

	vector< vector<MapCellType> > finalMap;

	// Initialize map cell type codes to special json datatype strings
	void initCodes()
	{
		// Matches program enums to output codes
		cellCodes[UNKNOWN] = "'(null)'";
		cellCodes[PLAINS_DEEP_MOUNTAIN] = "'pm'";
		cellCodes[DESERT_DEEP_MOUNTAIN] = "'dm'";
		cellCodes[SNOW_DEEP_MOUNTAIN] = "'sm'";
		cellCodes[PLAINS_LIGHT_MOUNTAIN] = "'pm'";
		cellCodes[DESERT_LIGHT_MOUNTAIN] = "'dm'";	
		cellCodes[SNOW_LIGHT_MOUNTAIN] = "'sm'";
		cellCodes[SHORE] = "'wb'";
		cellCodes[SEA] = "'ws'";
		cellCodes[OCEAN] = "'wo'";
		cellCodes[PLAINS] = "'pn'";
		cellCodes[DESERT] = "'dn'";
		cellCodes[SNOW] = "'sn'";
		cellCodes[PLAINS_FOREST] = "'pf'";
		cellCodes[DESERT_FOREST] = "'df'";
		cellCodes[SNOW_FOREST] = "'sf'";
		
		// Matches output codes to cell name in string
		outputCodes["'pn'"] = "plains";
		outputCodes["'dn'"] = "desert";
		outputCodes["'sn'"] = "snow";
		outputCodes["'pm'"] = "pmountain";
		outputCodes["'dm'"] = "dmountain";
		outputCodes["'sm'"] = "smountain";
		outputCodes["'pf'"] = "pforest";
		outputCodes["'df'"] = "dforest";
		outputCodes["'sf'"] = "sforest";
		outputCodes["'wb'"] = "beach";
		outputCodes["'ws'"] = "sea";
		outputCodes["'wo'"] = "ocean";
	}

	// Copies the 2D map generator to the array for use. Also sets min and max range values
	void copy(vector< vector<double> > & target, double & min, double & max)
	{
		min = gen.getMinValue();
		max = gen.getMaxValue();
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j)
			{
				target[i][j] = gen.getValue(GridPoint2D(i, j));
			}
	}
public:
	MapGenerator(int width, int height)
		: width(width), height(height), size(0),
		gen(), output(EARTHLIKE), cellCodes(), outputCodes(),
		elevationMap(width, vector<double>(height, 0.0)), elevationMin(0.0), elevationMax(0.0),
		temperatureMap(width, vector<double>(height, 0.0)), temperatureMin(0.0), temperatureMax(0.0),
		vegetationMap(width, vector<double>(height, 0.0)), vegetationMin(0.0), vegetationMax(0.0),
		finalMap(width, vector<MapCellType>(height, UNKNOWN))
	{
		// Determine the minimum size required for a map generator based on the width and height parameters
		// the reason for this is the 2D generator only works in powers of 2^n + 1.
		//
		// However, by checking the size, we allow any size width and height by cutting off excess from the
		// 2D generator.
		int size1 = 0;
		while (((width - 2) >> size1) >= 1)
			++size1;
		int size2 = 0;
		while (((height - 2) >> size2) >= 1)
			++size2;
		size = max(size1, size2);
		if (size < 0) size = 0;
		if (max(width, height) == 2) size = 1;

		initCodes();
	}

	// Runs the generators to populate all maps required
	//
	// gen.init requires the minimum size parameter
	// as well as an initial rate for jump value
	// and a decay rate for jump value
	void process()
	{
		gen.init(size, 3.00, 0.35);
		gen.reset();
		gen.process();
		copy(elevationMap, elevationMin, elevationMax);
		
		gen.init(size, 1.00, 0.50);
		gen.reset();
		gen.process();
		copy(temperatureMap, temperatureMin, temperatureMax);

		gen.init(size, 3.00, 0.25);
		gen.reset();
		gen.process();
		copy(vegetationMap, vegetationMin, vegetationMax);

		finalize();
	}

	// This function determines what kind of types the cells are by using the information maps generated.
	void finalize()
	{
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j)
			{
				double valElevation = (elevationMap[i][j] - elevationMin) / (elevationMax - elevationMin);
				double valTemperature = (temperatureMap[i][j] - temperatureMin) / (temperatureMax - temperatureMin);
				double valVegetation = (vegetationMap[i][j] - vegetationMin) / (vegetationMax - vegetationMin);
				if (valElevation <= ELEVATION_CHECK_HIGHMOUNTAINS)
				{
					if (valTemperature <= TEMPERATURE_CHECK_HOT)
						finalMap[i][j] = DESERT_DEEP_MOUNTAIN;
					else if (valTemperature <= TEMPERATURE_CHECK_WARM)
						finalMap[i][j] = PLAINS_DEEP_MOUNTAIN;
					else
						finalMap[i][j] = SNOW_DEEP_MOUNTAIN;
				}
				else if (valElevation <= ELEVATION_CHECK_LOWMOUNTAINS)
				{
					if (valTemperature <= TEMPERATURE_CHECK_HOT)
						finalMap[i][j] = DESERT_LIGHT_MOUNTAIN;
					else if (valTemperature <= TEMPERATURE_CHECK_WARM)
						finalMap[i][j] = PLAINS_LIGHT_MOUNTAIN;
					else
						finalMap[i][j] = SNOW_LIGHT_MOUNTAIN;
				}
				else if (valElevation <= ELEVATION_CHECK_FLATLAND)
				{
					if (valTemperature <= TEMPERATURE_CHECK_HOT)
					{
						if (valVegetation <= VEGETATION_CHECK_LUSH)
							finalMap[i][j] = DESERT_FOREST;
						else if (valVegetation <= VEGETATION_CHECK_NORMAL)
							finalMap[i][j] = DESERT;
						else
							finalMap[i][j] = DESERT;
					}
					else if (valTemperature <= TEMPERATURE_CHECK_WARM)
					{
						if (valVegetation <= VEGETATION_CHECK_LUSH)
							finalMap[i][j] = PLAINS_FOREST;
						else if (valVegetation <= VEGETATION_CHECK_NORMAL)
							finalMap[i][j] = PLAINS;
						else
							finalMap[i][j] = PLAINS;
					}
					else
					{
						if (valVegetation <= VEGETATION_CHECK_LUSH)
							finalMap[i][j] = SNOW_FOREST;
						else if (valVegetation <= VEGETATION_CHECK_NORMAL)
							finalMap[i][j] = SNOW;
						else
							finalMap[i][j] = SNOW;
					}
				}
				else if (valElevation <= ELEVATION_CHECK_SHORE)
					finalMap[i][j] = SHORE;
				else if (valElevation <= ELEVATION_CHECK_SEA)
					finalMap[i][j] = SEA;
				else
					finalMap[i][j] = OCEAN;
			}
	}

	// Switches draw modes to affect the graphical output function
	void switchDrawMode()
	{
		int val = static_cast<int>(output) + 1;
		if (val >= NUM_OUTPUT_TYPES)
			val = 0;
		output = static_cast<OutputType>(val);
	}

	// Draws the entire map using hardcoded colors, grayscale output is used to determine the output from the 2D generator
	void draw() const
	{
		double boxWidth = static_cast<double>(WINDOW_WIDTH) / width;
		double boxHeight = static_cast<double>(WINDOW_HEIGHT) / height;
		double wpadding = 0.0; //width / 10;
		double hpadding = 0.0; //height / 10;

		glBegin(GL_QUADS);
		for (int i = 0; i < width; ++i)
			for (int j = 0; j < height; ++j)
			{
				double valElevation = (elevationMap[i][j] - elevationMin) / (elevationMax - elevationMin);
				double valTemperature = (temperatureMap[i][j] - temperatureMin) / (temperatureMax - temperatureMin);
				double valVegetation = (vegetationMap[i][j] - vegetationMin) / (vegetationMax - vegetationMin);

				if (output == GRAYSCALE_ELEVATION)	
					glColor3d(valElevation, valElevation, valElevation);
				else if (output == GRAYSCALE_TEMPERATURE)
					glColor3d(valTemperature, valTemperature, valTemperature);
				else if (output == GRAYSCALE_VEGETATION)
					glColor3d(valVegetation, valVegetation, valVegetation);
				else
				{	
					switch (finalMap[i][j])
					{
					case PLAINS_DEEP_MOUNTAIN:
						glColor3d(0.5, 0.5, 0.5);
						break;
					case DESERT_DEEP_MOUNTAIN:
						glColor3d(0.5, 0.5, 0.5);
						break;
					case SNOW_DEEP_MOUNTAIN:
						glColor3d(0.5, 0.5, 0.5);
						break;
					case PLAINS_LIGHT_MOUNTAIN:
						glColor3d(0.8, 0.8, 0.8);
						break;
					case DESERT_LIGHT_MOUNTAIN:
						glColor3d(0.8, 0.8, 0.8);
						break;
					case SNOW_LIGHT_MOUNTAIN:
						glColor3d(0.8, 0.8, 0.8);
						break;
					case SHORE:
						glColor3d(1.0, 1.0, 0.5);
						break;
					case SEA:
						glColor3d(0.0, 0.8, 1.0);
						break;
					case OCEAN:
						glColor3d(0.0, 0.0, 1.0);
						break;
					case PLAINS:
						glColor3d(0.0, 1.0, 0.0);
						break;
					case DESERT:
						glColor3d(1.0, 1.0, 0.0);
						break;
					case SNOW:
						glColor3d(1.0, 1.0, 1.0);
						break;
					case PLAINS_FOREST:
						glColor3d(0.0, 0.5, 0.0);
						break;
					case DESERT_FOREST:
						glColor3d(0.2, 0.8, 0.0);
						break;
					case SNOW_FOREST:
						glColor3d(0.5, 0.8, 0.5);
						break;
					}
				}
				glVertex2d(i * boxWidth + wpadding, j * boxHeight + hpadding);
				glVertex2d(i * boxWidth + boxWidth - wpadding, j * boxHeight + hpadding);
				glVertex2d(i * boxWidth + boxWidth - wpadding, j * boxHeight + boxHeight - hpadding);
				glVertex2d(i * boxWidth + wpadding, j * boxHeight + boxHeight - hpadding);
			}
		glEnd();
	}

	// Outputs to the specified file represented in json format. It only outputs partial complete code from json.
	void outputToFile(const string & filename)
	{
		ofstream outfile(filename.c_str());

		outfile << "{\n"
				<< "\tkeys : {\n"; // m : 'mountains', p : 'plains', d : 'desert', s : 'snow', f : 'forest', b : 'beach', v : 'seas', w : 'ocean' },\n"

		for (map<string, string>::iterator it = outputCodes.begin(); it != outputCodes.end();)
		{
			outfile << "\t\t" << it->first << " : '" << it->second << "'";
			++it;
			if (it != outputCodes.end())
				outfile << ",";
			outfile << endl;
		}
		outfile << "\t},\n";

		outfile	<< "\twidth : " << width << ",\n"
				<< "\theight : " << height << ",\n";
		
		outfile << "\tmatrix : [\n";
		for (int j = 0; j < height; ++j)
		{
			outfile << "\t\t[";
			for (int i = 0; i < width; ++i)
			{					
				outfile << cellCodes[finalMap[i][j]];
				if (i != width - 1)
					outfile << ",";
			}
			outfile << "]";
				if (j != height - 1)
				outfile << ",";
			outfile << endl;
		}
		outfile << "\t]\n";
		outfile << "}\n";
		outfile.close();
	}

	// Reads from the specified file.
	void inputFromFile(const string & filename)
	{
		ifstream infile(filename.c_str());
		

		infile.close();
	}
};

MapGenerator mapGen(256, 256);

//Converts the provided point, p, from screen coordinates to OpenGL coordinate system 
void GLscreenToWindowCoordinates(double x, double y, double & rx, double & ry)
{
	//Obtain conversion values
	double model[16];
	glGetDoublev(GL_MODELVIEW_MATRIX, model);
	double projection[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT,viewport);

	double temp;
	//viewport[3] - p.y because of the flip on the y axis
	gluUnProject(x, viewport[3] - y, 0, model, projection, viewport, 
		&rx, &ry, &temp);
}

void GLrender();
void GLkeyboardPress(unsigned char key, int x, int y);
void GLprocessMouse(int button, int state, int x, int y);

int main(int argc, char** argv)
{
	srand(time(0));

	glutInit(&argc, argv);
	
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutCreateWindow("Map Generator");
	glutDisplayFunc(GLrender);
	glutKeyboardFunc(GLkeyboardPress);
	glutMouseFunc(GLprocessMouse);
	gluOrtho2D(0.0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0);

	glutMainLoop();

	return 0;
}

void GLrender()
{
	glClear(GL_COLOR_BUFFER_BIT); 

	//This is where we draw
	mapGen.draw();

	glFlush();	
	glutSwapBuffers();
}

void GLkeyboardPress(unsigned char key, int x, int y)
{
	if (key == 'z')
	{
		mapGen.switchDrawMode();
		glutPostRedisplay();
	}
	else if (key == 'x')
	{
		mapGen.outputToFile("output.txt");
	}
}

void GLprocessMouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		mapGen.process();
		glutPostRedisplay();
	}
}