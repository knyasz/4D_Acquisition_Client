#define PI	3.14159265358979323846f
#define N	32

typedef unsigned char uchar;
typedef unsigned short int uint16;
typedef unsigned int uint32;

//enum that represents constant value position
enum EDepthToWorld
{
	FX = 0,
	FY,
	CX,
	CY
};

//struct that contains the world data
struct SWorldPoint
{
	float x;
	float y;
	float z;

	SWorldPoint():
		x(0.f),
		y(0.f),
		z(0.f)
	{};

	SWorldPoint(const float& inX, const float& inY, const float& inZ):
		x(inX),
		y(inY),
		z(inZ)
	{};

	SWorldPoint(const SWorldPoint& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
	};

	SWorldPoint& operator=(const SWorldPoint& other)
	{
		this->x = other.x;
		this->y = other.y;
		this->z = other.z;

		return *this;
	}

};


float dtmGpu( uchar* in, uchar* out, const int rows, const int cols, const int minDist, const int maxDist);
void dtmGpuColor( uchar* in, uchar* inCol, uchar* out, const int rows, const int cols, const int minDist, const int maxDist);
void depthToRgbWorldPoint(uchar* in, float* transMat, uchar* out, const int rows, const int cols, const int minDist, const int maxDist);

