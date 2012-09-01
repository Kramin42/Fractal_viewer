#include "Slider.h"
#include "SDL.h"
#include "SDL_gfxPrimitives.h"

Slider::Slider()
{
    bars[0]=0.0;
    bars[1]=0.5;
    bars[2]=1.0;
    xpos=0;
    ypos=0;
    w=0;
    h=0;
    selection=0;
    barWidth = 10;
}

Slider::Slider(double Start, double Middle, double End, int XPos, int YPos, int Width, int Height)
{
    bars[0]=Start;
    bars[1]=Middle;
    bars[2]=End;
    xpos=XPos;
    ypos=YPos;
    w=Width;
    h=Height;
    selection=0;
    barWidth = 10;
}

void Slider::select(int Selection)
{
    selection=Selection;
}

void Slider::deselect()
{
    selection = 0;
}

bool Slider::isSelected()
{
    return selection != 0;
}

bool Slider::handleClick(int mouseX, int mouseY)
{
    if (mouseX>=xpos && mouseX<xpos+w && mouseY>=ypos && mouseY<ypos+h) // was the click inside the slider?
    {
//        double normMX = (double) (mouseX - xpos)/(double) w;
//        double minDistSq = 10.0;
//        int index = -1;
//        for (int i=0;i<3;i++){
//            if ((normMX-bars[i])*(normMX-bars[i])<minDistSq)
//            {
//                minDistSq=(normMX-bars[i])*(normMX-bars[i]);
//                index=i;
//            }
//        }
//        if (index == -1) return false;
//        selection = index+1;
        for (int i=0;i<3;i++)
        {
            int sliderXPos = (int)(bars[i]*w) + xpos;
            if (mouseX>=sliderXPos-barWidth && mouseX<=sliderXPos+barWidth)
            {
                selection = i+1;
                return true;
            }
        }
    }
    return false;
}

void Slider::updateSlider(int mouseX)
{
    double normMX = (double) (mouseX - xpos)/(double) w;
    switch (selection)
    {
    case 1:
        if (normMX>=0 && normMX<bars[1])
        {
            bars[0] = normMX;
        }
        break;
    case 2:
        if (normMX>bars[0] && normMX<bars[2])
        {
            bars[1] = normMX;
        }
        break;
    case 3:
        if (normMX>bars[1] && normMX<=1.0)
        {
            bars[2] = normMX;
        }
        break;
    default:
        break;
    }
}

void Slider::drawSlider(SDL_Surface* screen)
{
    for (int i=0;i<3;i++)
    {
        int sliderXPos = (int)(bars[i]*w) + xpos;
        rectangleColor(screen,sliderXPos-barWidth,ypos,sliderXPos+barWidth,ypos+h,0xFFFFFF60);
        lineColor(screen,sliderXPos,ypos,sliderXPos,ypos+h,0xFFFFFFFF);
    }
}
