
#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>

using uint = unsigned int;
using byte = uint_fast8_t;

struct Colour
{
private:
	bool _alpha;
public:
	byte R;
	byte G;
	byte B;
	byte A;
	Colour()
	{
		R = 0x00;
		G = 0x00;
		B = 0x00;
		A = 0x00;
		_alpha = false;
	}
	Colour(byte v)
	{
		R = v;
		G = v;
		B = v;
		A = 0xFF;
		_alpha = false;
	}
	Colour(byte r, byte g, byte b)
	{
		R = r;
		G = g;
		B = b;
		A = 0xFF;
		_alpha = false;
	}
	Colour(uint colour, bool alpha)
	{
		_alpha = alpha;
		if (_alpha)
		{
			R = (byte)(colour >> 24);
			G = (byte)(colour >> 16);
			B = (byte)(colour >> 8);
			A = (byte)(colour);
		}
		else
		{
			R = (byte)(colour >> 16);
			G = (byte)(colour >> 8);
			B = (byte)(colour);
			A = 0xFF;
		}
	}
	uint get()
	{
		uint col = 0;
		if (_alpha)
		{
			col += (uint)(R << 24);
			col += (uint)(G << 16);
			col += (uint)(B << 8);
			col += (uint)(A);
		}
		else
		{
			col += (uint)(R << 16);
			col += (uint)(G << 8);
			col += (uint)(B);
		}
		return col;
	}
	void multiplyBy(double value)
	{
		R *= value;
		G *= value;
		B *= value;
		A *= value;
	}
	void Add(double value)
	{
		R += value;
		G += value;
		B += value;
		A += value;
	}
	void Add(uint value)
	{
		if (_alpha)
		{
			R += (byte)(value >> 16);
			G += (byte)(value >> 8);
			B += (byte)value;
		}
		else
		{
			R += (byte)(value >> 24);
			G += (byte)(value >> 16);
			B += (byte)(value >> 8);
			A += (byte)(value);
		}
	}
};


class IMG
{
private:
	uint _width;
	uint _height;
	uint _bytesPerPixel;
	bool _alpha;
	Colour* _data;

public:
	IMG(uint width, uint height, bool alpha)
	{
		_width = width;
		_height = height;
		_alpha = alpha;
		_bytesPerPixel = 3;
		if (_alpha)
			_bytesPerPixel = 4;
		_data = new Colour[_width * _height];
		for (uint i = 0; i < _width * _height; i++)
		{
			_data[i] = Colour(0, _alpha);
		}
	}
	byte* asBytes()
	{
		byte* byteData = new byte[_width * _height * _bytesPerPixel];
		int byteIndex = 0;
		for (int i = 0; i < _width * _height; i++)
		{
			byteData[byteIndex] = _data[i].B;
			byteData[byteIndex + 1] = _data[i].G;
			byteData[byteIndex + 2] = _data[i].R;
			if (_alpha)
			{
				byteData[byteIndex + 3] = _data[i].A;
				byteData += 4;
			}
			else
				byteIndex += 3;
		}
		return byteData;
	}
	byte* asBytes(bool withAlpha)
	{
		byte* byteData = new byte[_width * _height * _bytesPerPixel];
		int byteIndex = 0;
		for (int i = 0; i < _width * _height; i++)
		{
			byteData[byteIndex] = (byte)_data[i].B;
			byteData[byteIndex + 1] = (byte)_data[i].G;
			byteData[byteIndex + 2] = (byte)_data[i].R;
			if (withAlpha)
			{
				if (_alpha)
					byteData[byteIndex + 3] = (byte)_data[i].A;
				else
					byteData[byteIndex + 3] = (byte)0xFF;
				byteData += 4;
			}
			else
				byteIndex += 3;
		}
		return byteData;
	}

	void pset(uint x, uint y, Colour colour)
	{
		
		if (x >= _width || y >= _height)
		{
			std::cout << "coord out of range at x=" << x << " y=" << y << std::endl;
		}
		//std::cout << x << " " << y << " " << _height << std::endl;
		_data[(y * _width) + x] = colour;
	}

};

class BMP
{
private:
	static const int FILE_HEADER_BYTES = 14;
	static const int INFO_HEADER_BYTES = 40;
	uint _width;
	uint _height;
	uint _bytesPerPixel;
	uint _padding;
	byte _fileHeader[FILE_HEADER_BYTES];
	byte _infoHeader[INFO_HEADER_BYTES];
	byte* _data;

public:
	BMP(uint width, uint height)
	{
		_width = width;
		_height = height;
		_bytesPerPixel = 3;

		_data = new byte[_width * _height * _bytesPerPixel];
		for (uint i = 0; i < _width * _height * _bytesPerPixel; i++)
		{
			_data[i] = 0xFF;
		}

		uint dataWidth = _width * _bytesPerPixel;
		_padding = 4 - (dataWidth % 4);
		if (dataWidth % 4 == 0)
			_padding = 0;
		dataWidth += _padding;
		uint filesize = dataWidth * height;
		filesize += INFO_HEADER_BYTES + FILE_HEADER_BYTES;

		//FILE HEADER

		//image format
		_fileHeader[0] = 'B';
		_fileHeader[1] = 'M';
		//filesize in bytes
		_fileHeader[2] = (byte)filesize;
		_fileHeader[3] = (byte)(filesize >> 8);
		_fileHeader[4] = (byte)(filesize >> 16);
		_fileHeader[5] = (byte)(filesize >> 24);
		//reserved
		_fileHeader[6] = 0;
		_fileHeader[7] = 0;
		_fileHeader[8] = 0;
		_fileHeader[9] = 0;
		//headersize
		uint headerSize = FILE_HEADER_BYTES + INFO_HEADER_BYTES;
		_fileHeader[10] = (byte)headerSize;
		_fileHeader[11] = (byte)(headerSize >> 8);
		_fileHeader[12] = (byte)(headerSize >> 16);
		_fileHeader[13] = (byte)(headerSize >> 24);


		//INFO HEADER

		//info header size
		_infoHeader[0] = (byte)INFO_HEADER_BYTES;
		_infoHeader[1] = (byte)(INFO_HEADER_BYTES >> 8);
		_infoHeader[2] = (byte)(INFO_HEADER_BYTES >> 16);
		_infoHeader[3] = (byte)(INFO_HEADER_BYTES >> 24);
		//image width
		_infoHeader[4] = (byte)width;
		_infoHeader[5] = (byte)(width >> 8);
		_infoHeader[6] = (byte)(width >> 16);
		_infoHeader[7] = (byte)(width >> 24);
		//image height
		_infoHeader[8] = (byte)height;
		_infoHeader[9] = (byte)(height >> 8);
		_infoHeader[10] = (byte)(height >> 16);
		_infoHeader[11] = (byte)(height >> 24);
		//no. of planes (must be zero)
		_infoHeader[12] = 0;
		_infoHeader[13] = 0;
		//bits per pixel
		_infoHeader[14] = (byte)(_bytesPerPixel * 8);
		_infoHeader[15] = (byte)((_bytesPerPixel * 8) >> 8);
		//type of compression (0 - no compression)
		_infoHeader[16] = 0;
		_infoHeader[17] = 0;
		_infoHeader[18] = 0;
		_infoHeader[19] = 0;
		//size of image data (no compression -> 0)
		_infoHeader[20] = 0;
		_infoHeader[21] = 0;
		_infoHeader[22] = 0;
		_infoHeader[23] = 0;
		//horizonal pixels per meter on target device (usually set to zero)
		_infoHeader[24] = 0;
		_infoHeader[25] = 0;
		_infoHeader[26] = 0;
		_infoHeader[27] = 0;
		//vertical pixels per meter on target device (usually set to zero)
		_infoHeader[28] = 0;
		_infoHeader[29] = 0;
		_infoHeader[30] = 0;
		_infoHeader[31] = 0;
		//no. of colours used, if zero, number is calculated;
		_infoHeader[32] = 0;
		_infoHeader[33] = 0;
		_infoHeader[34] = 0;
		_infoHeader[35] = 0;
		//no. of important colours, if zero all are important
		_infoHeader[36] = 0;
		_infoHeader[37] = 0;
		_infoHeader[38] = 0;
		_infoHeader[39] = 0;
	};

	void saveImage(const char* name)
	{
		std::ofstream outImage(name, std::ios::out | std::ios::binary);
		if (outImage)
		{
			outImage.write((char*)_fileHeader, FILE_HEADER_BYTES);
			outImage.write((char*)_infoHeader, INFO_HEADER_BYTES);

			uint dataSize = (_height * ((_width * _bytesPerPixel) + _padding));
			byte* paddedData = new byte[dataSize];


			uint pads = 0;
			for (uint i = 0; i < dataSize; i++)
			{
				uint modWidth = i % (dataSize / _height);
				if (modWidth <= (dataSize / _height) - 1 && modWidth >= (dataSize / _height) - _padding)
				{
					pads++;
					paddedData[i] = 0x00;
				}
				else
				{
					paddedData[i] = _data[i - (pads)];
				}
			}
			outImage.write((char*)paddedData, dataSize);

			outImage.close();
		}
	}

	void pset(uint x, uint y, byte r, byte g, byte b)
	{
		if (x >= _width || y >= _height)
		{
			std::cout << "coord out of range at:   x:" << x << "  y:" << y << std::endl;
			return;
		}
		uint posOfP = (y * (_width * _bytesPerPixel)) + (x * _bytesPerPixel);
		_data[posOfP] = r;
		_data[posOfP + 1] = g;
		_data[posOfP + 2] = b;
	}
	void pset(uint x, uint y, uint rgb)
	{
		//check x and y is within image
		if (x >= _width || y >= _height)
		{
			std::cout << "coord out of range at:   x:" << x << "  y:" << y << std::endl;
			return;
		}
		//get the index of the first byte of the pixel
		uint posOfP = (y * (_width * _bytesPerPixel)) + (x * _bytesPerPixel);
		_data[posOfP] = (byte)(rgb);
		_data[posOfP + 1] = (byte)(rgb >> 8);
		_data[posOfP + 2] = (byte)(rgb >> 16);
	}

	void setData(byte* data)
	{
		_data = data;
	}
};



double gradientValue(double index, double total)
{
	double val = abs(sin((((double)index / total) * (3.1415 / 2))));
	return pow(val, 2.2);
}

Colour gradient(Colour col1, Colour col2, double intensity)
{
	col1.multiplyBy(intensity);
	col2.multiplyBy(1 - intensity);
	col1.R = (col1.R + col2.R);
	col1.G = (col1.G + col2.G);
	col1.B = (col1.B + col2.B);
	return col1;
}

uint getValue255lim()
{
	uint val = 255;
	std::string inp = "";
	std::cin >> inp;
	try
	{
		val = std::stoi(inp);
	}
	catch (std::invalid_argument)
	{
		std::cout << "error: invalid input" << std::endl;
		val = 255;
	}
	catch (std::out_of_range)
	{
		std::cout << "error: out of range" << std::endl;
		val = 255;
	}
	if (val > 255)
		val = 255;
	return val;
}
uint getValue()
{
	uint val = 1000;
	std::string inp = "";
	std::cin >> inp;
	try
	{
		val = std::stoi(inp);
	}
	catch (std::invalid_argument)
	{
		std::cout << "error: invalid input" << std::endl;
		val = 1000;
	}
	catch (std::out_of_range)
	{
		std::cout << "error: out of range" << std::endl;
		val = 1000;
	}
	return val;
}

int main()
{
	
	uint imageWidth(1000), imageHeight(1000);

	std::cout << "image width: ";
	imageWidth = getValue();
	std::cout << "image height: ";
	imageHeight = getValue();

	IMG img(imageWidth, imageHeight, false);
	
	Colour grad1, grad2;

	std::cout << "please enter the colour value for the first gradient" << std::endl;
	std::cout << "R: ";
	grad1.R = getValue255lim();
	std::cout << "G: ";
	grad1.G = getValue255lim();
	std::cout << "B: ";
	grad1.B = getValue255lim();
	grad1.A = 0xFF;

	std::cout << "please enter the colour value for the second gradient" << std::endl;
	std::cout << "R: ";
	grad2.R = getValue255lim();
	std::cout << "G: ";
	grad2.G = getValue255lim();
	std::cout << "B: ";
	grad2.B = getValue255lim();
	grad2.A = 0xFF;
	

	std::cout << "do you want a horizonal or vertical gradeint? (h/v)" << std::endl;
	bool horizonal = true;
	std::string ans = "";
	std::cin >> ans;
	if (ans == "v" || ans == "V")
		horizonal = false;
	else if (ans != "h" && ans != "H")
		std::cout << "incorrect input, defaulted to horizontal" << std::endl;


	for (uint i = 0; i < imageWidth; i++)
	{
		for (uint j = 0; j < imageHeight; j++)
		{
			Colour col((byte)0xFF);
			double x = 0;
			if (horizonal)
				x = gradientValue(i, imageWidth);
			else
				x = gradientValue(j, imageHeight);
			col = gradient(grad2, grad1, x);
			img.pset(i, j, col);
		}
	}
	BMP bmp(imageWidth, imageHeight);
	bmp.setData(img.asBytes());
	std::cout << "name of file: ";
	std::string name = "";
	std::cin >> name;
	name += ".bmp";
	bmp.saveImage(name.c_str());

	return 0;
}