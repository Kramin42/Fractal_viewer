#ifndef SLIDER_H_INCLUDED
#define SLIDER_H_INCLUDED

#include "SDL.h"

class Slider
{
private:

public:
    int xpos, ypos; // position of the slider
    int w, h;       // width and height of the slider
    double bars[3];   // start, middle, and end positions
    int selection;
    int barWidth; // width of the bars on the slider
    Slider();
    Slider(double Start, double Middle, double End, int XPos, int YPos, int Width, int Height);
    void select(int Selection);
    bool handleClick(int mouseX, int mouseY); // checks if the slider was clicked and reacts appropriately
    void deselect();
    bool isSelected();
    void updateSlider(int mouseX);
    void drawSlider(SDL_Surface* screen);
};

#endif // SLIDER_H_INCLUDED
