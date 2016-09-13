#pragma once
struct Colors
{
public:
	Colors(unsigned int r, unsigned int g, unsigned int b, unsigned int alfa);
	unsigned int fRGBa[3];
};

/* colors.cpp */

Colors::Colors(unsigned int r, unsigned int g, unsigned int b, unsigned int alfa)
{
	fRGBa[0] = r;
	fRGBa[1] = g;
	fRGBa[2] = b;
	fRGBa[3] = alfa;
};

class ColorsPalette
{
public:
	//rule of three
	ColorsPalette();
	ColorsPalette(const ColorsPalette &other) = default;
	const ColorsPalette &operator=(ColorsPalette rhs) = delete;
	~ColorsPalette() {};

	//methods
	void initColorMap();
	Colors* getColorFromMap(char color);


private:
	std::map<char, Colors* >::iterator it;  //Map iterator
	std::map< char, Colors* > mapColors;	//ColorMap
};

//rule of three
ColorsPalette::ColorsPalette() 
{
	initColorMap();
};

//methods
Colors* ColorsPalette::getColorFromMap(char color)
{
	it = mapColors.find(color);
	if (it == mapColors.end())
	{
		std::cout << "No Colours Found" << std::endl;
		return mapColors.find('w')->second;				// return white if error
	}
	return it->second;
}

void ColorsPalette::initColorMap()
{
	mapColors['r'] = new Colors(1, 0, 0, 1);	    //red
	mapColors['g'] = new Colors(0, 1, 0, 1);	    //green
	mapColors['b'] = new Colors(0, 0, 1, 1);	    //blue
	mapColors['w'] = new Colors(1, 1, 1, 1);        //white
	mapColors['B'] = new Colors(0, 0, 0, 1);	    //Black

}