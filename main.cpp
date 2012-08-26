//The headers
#include "SDL.h"
#include "SDL_gfxPrimitives.h"
#include "SDL_ttf.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace std;

//function prototypes


//Screen attributes
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int SCREEN_BPP = 32;

// Get the current video hardware information
//const SDL_VideoInfo* VideoInfo = SDL_GetVideoInfo();

//other constants
const float PI = atan(1.0f) * 4.0f;
const bool  FULLSCREEN = false;

//usefull vars
int i,j;
int w=SCREEN_WIDTH,h=SCREEN_HEIGHT;
double wd=w,hd=h;

int start,end;
int widthPerFrame = SCREEN_WIDTH/100;
bool redraw;

int mx,my,mdownx,mdowny;
bool dragging = false;

double x,y,ratioX,ratioY;

//Event handler
SDL_Event event;

SDL_Surface *screen = NULL;

Uint8 *keys = NULL;

float keyChangeMod = 1.0f;

//The font that's going to be used
TTF_Font *font = NULL;
int fontSize = 14;

//The color of the font
SDL_Color textColor = { 255, 255, 255 };

//the SDL Surfaces for putting text into
SDL_Surface *textMaxIteration = NULL;
SDL_Surface *textColMult = NULL;
SDL_Surface *textTransformPower = NULL;
SDL_Surface *textDistanceDivide = NULL;
SDL_Surface *textDDA = NULL;
SDL_Surface *textDDB = NULL;
SDL_Surface *textOther = NULL;
SDL_Surface *textLeftShift = NULL;
SDL_Surface *textLeftCtrl = NULL;

SDL_Surface *text = NULL;

string s;
stringstream out;

Uint32 data[SCREEN_WIDTH*SCREEN_HEIGHT];

double mus[SCREEN_WIDTH*SCREEN_HEIGHT];
bool isblack[SCREEN_WIDTH*SCREEN_HEIGHT];

//default values
double M_centerX = -0.75;
double M_centerY = 0.0;
double M_zoom = 1.0;
int M_maxIteration = 200;
int M_ColMult = 1;
double M_bailoutRad = 16.0;
double BRsq = M_bailoutRad*M_bailoutRad;
double transformPower = 0.4;
bool distanceDivide = false;
double ddA = 0.4;
double ddB = 0.5;
//    int iValue = (int) value;
//    if (((double) rand()/(double) RAND_MAX)>(value - (double) iValue))
//    {
//        iValue++;
//    }
//    return iValue;
//}

float getColorValue(double mu, double s, double m, double e)
{
    if (mu<s) {
        return 0.0f;
    } else if (mu<m) {
        return (mu - s)/(m-s);
    } else if (mu<e) {
        return 1.0f - (mu - m)/(e-m);
    }
    return 0.0f;
}

Uint32 getUintfromRGB(Uint8 r, Uint8 g, Uint8 b)
{
    return b + (g<<8) + (r<<16);
}
//    //Convert the pixels to 32 bit
//    Uint32 *pixels = (Uint32 *)surface->pixels;
//
//    //Set the pixel
//    pixels[ ( y * surface->w ) + x ] = pixel;
//}

void renderText()
{
    out.str("");
    out << "Max Iteration(Q,A): " << M_maxIteration;
    s = out.str();
    textMaxIteration = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "Colour multiplier(W,S): " << M_ColMult;
    s = out.str();
    textColMult = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "Transform Power(E,D): " << transformPower;
    s = out.str();
    textTransformPower = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "Distance Divide(C): " << distanceDivide;
    s = out.str();
    textDistanceDivide = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "DistDivide A(T,G): " << ddA;
    s = out.str();
    textDDA = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "DistDivide B(Y,H): " << ddB;
    s = out.str();
    textDDB = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "Zoom out: Z, Save: X, Redraw: Space";
    s = out.str();
    textOther = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "change x10: left Shift";
    s = out.str();
    textLeftShift = TTF_RenderText_Solid(font,s.c_str(),textColor);
    out.str("");
    out << "change /10: left Ctrl";
    s = out.str();
    textLeftCtrl = TTF_RenderText_Solid(font,s.c_str(),textColor);
}

void checkKeys()
{

}

void save()
{
    ofstream myfile;
    myfile.open ("saves.txt",fstream::app);
    myfile << fixed << setprecision (16) <<"-CX "<<M_centerX<<" -CY "<<M_centerY<<" -MI "<<M_maxIteration<<" -CM "<<M_ColMult<<" -ZM "<<M_zoom<<" -TP "<<transformPower<<" -DD "<<distanceDivide<<" -DA "<<ddA<<" -DB "<<ddB<<endl;
    myfile.close();
}

void calcMandelbrot(int s, int e)
{
    double x0,y0;
    double x,y;
    double xsq,ysq;
    double mu;
    int iteration;

    //Algorithm for Mandelbrot set
    for (i=s;i<e;i++){
        for (j=0;j<h;j++){
            x0 = (((double) i)/wd)*(2.0/M_zoom)*(wd/hd) - ((1.0/M_zoom)*(wd/hd)-M_centerX);
            y0 = (((double) j)/hd)*(2.0/M_zoom) - ((1.0/M_zoom)+M_centerY);

            x = 0;
            y = 0;
            xsq=0;
            ysq=0;

            iteration = 0;
            do
            {
                iteration++;
                y = 2*x*y + y0;
                x = xsq - ysq + x0;

                xsq=x*x;
                ysq=y*y;
            }
            while(xsq + ysq < BRsq && iteration < M_maxIteration);

            if (iteration == M_maxIteration) {
                //color = 0xFF000000;
                isblack[w*j+i]=true;
            }
            else {
                isblack[w*j+i]=false;
                mu = (double)iteration + 1.0 - log(log(sqrt(x*x + y*y)))/log(2.0);
                mu = mu/(double)M_maxIteration;
                if (mu>=1.0f)
                {
                    cout<<mu<<endl;
                }
                mus[j*w+i]=mu;


                //color = 0xFFFFFFFF;
            }

            //imageG2D.setColor(color);
            //imageG2D.drawLine(i, j, i, j);
        }
    }
}

void drawMandelbrot(int s, int e)
{
    double mu;
    double divider;
    Uint32 color;
    double R,G,B;

    for (i=s;i<e;i++){
        for (j=0;j<h;j++){
            if (isblack[j*w+i]){
                color=0xFF000000;
            } else {
                mu = mus[j*w+i];
                mu=pow(mu,transformPower);
                divider  = mu;
                mu = mu*M_ColMult;
                mu = mu - floor(mu);

                //mu=(double)(w*j + i)/(double)(w*h);

                R=0;
                G=0;
                B=0;

                B = getColorValue(mu,0,0.5,0.6);
                G = getColorValue(mu,0.3,0.5,0.7);
                R = getColorValue(mu,0.4,0.5,1);

                if (distanceDivide)
                {
                    if (divider<ddA) {
                        R=0;
                        G=0;
                        B=0;
                    } else if (divider<ddB) {
                        R*=(divider-ddA)/(ddB-ddA);
                        G*=(divider-ddA)/(ddB-ddA);
                        B*=(divider-ddA)/(ddB-ddA);
                    }
                }

                color = getUintfromRGB((Uint8)(R*255),(Uint8)(G*255),(Uint8)(B*255));
            }
            data[j*w + i]=color;
        }
    }
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

/* render the scene */
void draw() {
    //Convert the pixels to 32 bit
    Uint32 *pixels = (Uint32 *)screen->pixels;

    for (i=0;i<w*h;i++)
    {
        pixels[i]=data[i];
    }

    apply_surface(0,0,textMaxIteration,screen);
    apply_surface(0,fontSize,textColMult,screen);
    apply_surface(0,2*fontSize,textTransformPower,screen);
    apply_surface(0,3*fontSize,textDistanceDivide,screen);
    apply_surface(0,4*fontSize,textDDA,screen);
    apply_surface(0,5*fontSize,textDDB,screen);
    apply_surface(0,6*fontSize,textOther,screen);
    apply_surface(0,7*fontSize,textLeftShift,screen);
    apply_surface(0,8*fontSize,textLeftCtrl,screen);

    if (dragging)
    {
        rectangleColor(screen,2*mdownx-mx,2*mdowny-my,mx,my,0xFFFFFFFF);
        lineColor(screen,2*mdownx-mx,mdowny,mx,mdowny,0xFFFFFFFF);
        lineColor(screen,mdownx,2*mdowny-my,mdownx,my,0xFFFFFFFF);
    }
}

bool init()
{
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        return false;
    }

    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;
    }

    Uint32 flags = SDL_SWSURFACE;
    if (FULLSCREEN)
    {
        flags|=SDL_FULLSCREEN;
    }

    //Create Window
    if( (screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, flags)) == NULL )
    {
        cout << "window creation failed" << endl;
        return false;
    }

    //grab the mouse
    //SDL_WM_GrabInput( SDL_GRAB_ON);

    //Set caption
    SDL_WM_SetCaption( "SDL Fractal", NULL );

    //Open the font
    font = TTF_OpenFont( "LiberationSans.ttf", fontSize );
    if (font == NULL)
    {
        cout << "font loading error" << endl;
        return false;
    }

    renderText();

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    return true;
}

void clean_up()
{
    //Quit SDL
    SDL_Quit();
}

int main( int argc, char *argv[] )
{
    srand(SDL_GetTicks());

    cout << "argc = " << argc << endl;
    for(i = 0; i < argc; i++)
        cout << "argv[" << i << "] = " << argv[i] << endl;
    for (i=1;i<argc-1;i++)
    {
            if(!strcmp(argv[i], "-CX")){
                M_centerX = atof(argv[i+1]);
                cout << "CX: "<<M_centerX<<endl;
            }
            else if(!strcmp(argv[i], "-CY")){
                M_centerY = atof(argv[i+1]);
                cout << "CY: "<<M_centerY<<endl;
            }
            else if(!strcmp(argv[i], "-MI")){
                M_maxIteration = atoi(argv[i+1]);
                cout << "MI: "<<M_maxIteration<<endl;
            }
            else if(!strcmp(argv[i], "-CM")){
                M_ColMult = atoi(argv[i+1]);
                cout << "CM: "<<M_ColMult<<endl;
            }
            else if(!strcmp(argv[i], "-ZM")){
                M_zoom = atof(argv[i+1]);
                cout << "ZM: "<<M_zoom<<endl;
            }
            else if(!strcmp(argv[i], "-TP")){
                transformPower = atof(argv[i+1]);
                cout << "TP: "<<transformPower<<endl;
            }
            else if(!strcmp(argv[i], "-DD")){
                distanceDivide = (bool)atoi(argv[i+1]);
                cout << "DD: "<<distanceDivide<<endl;
            }
            else if(!strcmp(argv[i], "-DA")){
                ddA = atof(argv[i+1]);
                cout << "DA: "<<ddA<<endl;
            }
            else if(!strcmp(argv[i], "-DB")){
                ddB = atof(argv[i+1]);
                cout << "DB: "<<ddB<<endl;
            }
            else if(!strcmp(argv[i], "-IW")){
                w = atoi(argv[i+1]);
                wd = w;
                cout << "IW: "<<w<<endl;
            }
            else if(!strcmp(argv[i], "-IH")){
                h = atoi(argv[i+1]);
                hd = h;
                cout << "IH: "<<h<<endl;
            }

    }

    //Quit flag
    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    keys = SDL_GetKeyState(NULL);

    redraw = true;

    //drawMandelbrot(0,w);
    //cout<<"done"<<endl;

	//main loop
	while( quit == false )
	{
	    bool keyPressed = false;
        //While there are events to handle
		while( SDL_PollEvent( &event ) )
		{
		    keyChangeMod=1.0f;
		    //check for SHIFT and CONTROL modifiers
            if (keys[SDLK_LSHIFT]){keyChangeMod=10.0f;}
            else if (keys[SDLK_LCTRL]){keyChangeMod=0.1f;}

			if( event.type == SDL_QUIT )
			{
                quit = true;
            }
            else if (event.type == SDL_KEYDOWN)
            {
                keyPressed = true;
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_r://its annoying when you accidentily press r so I removed it
                        //M_centerX = -0.75;
                        //M_centerY = 0;
                        //M_zoom = 1;
                        //redraw = true;
                        //start = 0;
                        //end = 0;
                        break;
                    case SDLK_z:
                        M_zoom /=1.0f+keyChangeMod;
                        redraw = true;
                        start = 0;
                        end = 0;
                        break;
                    case SDLK_SPACE:
                        redraw = true;
                        start = 0;
                        end = 0;
                        break;
                    case SDLK_q:
                        if (M_maxIteration<1000){M_maxIteration+=(int)(10*keyChangeMod);}
                        else {M_maxIteration+=(int)(100*keyChangeMod);}
                        break;
                    case SDLK_a:
                        if (M_maxIteration<1000){M_maxIteration-=(int)(10*keyChangeMod);}
                        else {M_maxIteration-=(int)(100*keyChangeMod);}
                        break;
                    case SDLK_w:
                        M_ColMult++;
                        break;
                    case SDLK_s:
                        M_ColMult--;
                        break;
                    case SDLK_e:
                        transformPower+=0.01*keyChangeMod;
                        break;
                    case SDLK_d:
                        transformPower-=0.01*keyChangeMod;
                        break;
                    case SDLK_t:
                        ddA+=0.01*keyChangeMod;
                        break;
                    case SDLK_g:
                        ddA-=0.01*keyChangeMod;
                        break;
                    case SDLK_y:
                        ddB+=0.01*keyChangeMod;
                        break;
                    case SDLK_h:
                        ddB-=0.01*keyChangeMod;
                        break;
                    case SDLK_c:
                        distanceDivide=!distanceDivide;
                        break;
                    case SDLK_x:
                        save();
                        break;
                    default: ;
                }
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                mx = event.motion.x;
                my = event.motion.y;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                mdownx = event.button.x;
                mdowny = event.button.y;
                dragging = true;
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                mx = event.button.x;
                my = event.button.y;
                if (mx>mdownx && my>mdowny)
                {
                    x = (((double) mdownx)/wd)*(2.0/M_zoom)*(wd/hd) - ((1.0/M_zoom)*(wd/hd)-M_centerX);
                    y = -((((double) mdowny)/hd)*(2.0/M_zoom) - ((1.0/M_zoom)+M_centerY));

                    ratioX = 2*(mx-mdownx)/wd;
                    ratioY = 2*(my-mdowny)/hd;
                    //cout << ratioX << " , " << ratioY << endl;

                    if (ratioX>=ratioY)
                    {
                        M_zoom /= ratioX;
                    }
                    else
                    {
                        M_zoom /= ratioY;
                    }
                    M_centerX = x;
                    M_centerY = y;

                    redraw = true;
                    start = 0;
                    end = 0;
                }
                dragging = false;
            }
		}

		if (keyPressed)
		{
            renderText();
            drawMandelbrot(0,w);
		}

		if (redraw)
		{
		    start = end;
		    end = start + widthPerFrame;
		    if (end>w)
		    {
		        end = w;
		        redraw = false;
		        //return 0; //uncomment to make the program quit after running once
		    }
		    calcMandelbrot(start,end);
		    drawMandelbrot(start,end);
		}

	    //draw the graphics
	    draw();

	    //Update screen
	    SDL_Flip( screen );

	    checkKeys();
	}

	//Clean up
	clean_up();

	return 0;
}
