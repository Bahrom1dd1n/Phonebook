#ifndef _Objects_

#define _Objects_

#include<SDL.h>
#include <SDL_ttf.h>
#include"Header/Person.h"

//Main data
SDL_Renderer* MainRenderer;// main renderer window

const SDL_Color ScreenColor = { 255,255,255,255 };

class Object :public SDL_Rect//interface for all objects
{
private:
    static int ObjectsCount;
public:
    Object() // Always constructor of base class called first
    {
        ObjectsCount++;
    }

    void SetRect(short x, short y, short width, short height)
    {
        this->x = x; this->y = y;
        w = width; h = height;
    }

    bool Contain(short x, short y)
    {
        if (this->x > x || x > this->x + w || this->y > y || y > this->y + h)
            return false;
        return true;
    }

    virtual void Render() = 0;

    virtual ~Object()
    {
        ObjectsCount--;
    }; // To make derivaed objects destructor called first

};

int Object::ObjectsCount = 0;

class TextureRect :public Object
{
protected:
    SDL_Texture* texture = nullptr;
public:
    TextureRect() {};

    TextureRect(SDL_Surface* surface, int x, int y)
    {
        this->x = x;
        this->y = y;
        if(surface){
            w = surface->w;
            h = surface->h;
        }
        texture = SDL_CreateTextureFromSurface(MainRenderer, surface);// creturing texture form that image
    }

    void Set(SDL_Surface* surface, int x, int y)
    {
        this->x = x;
        this->y = y;

        ChangeTexture(surface);
    }

    void ChangeTexture(SDL_Surface* surface)
    {
        if (texture)
            SDL_DestroyTexture(texture);

        if (!surface)
        {
            w = h = 0;
            texture = nullptr;
            return;
        }

        w = surface->w; h = surface->h;

        texture = SDL_CreateTextureFromSurface(MainRenderer, surface);// creturing texture form that image
    }

    void SetImage(const char *ImagePath,bool Trancparent=false)
    {

        SDL_Surface* img = SDL_LoadBMP(ImagePath);
        if(Trancparent)
            SDL_SetColorKey(img, SDL_TRUE, SDL_MapRGB(img->format, 255, 255,255));
        ChangeTexture(img);
    }

    void SetText(const char* text, const char* Font, int FontSize = 18, SDL_Color FontColor = { 0,0,0,255 })
    {
        //Do change font size 
        TTF_Font* font = TTF_OpenFont(Font, FontSize);
        SDL_Surface* img = TTF_RenderText_Solid(font, text, FontColor);
        ChangeTexture(img);
        SDL_FreeSurface(img);
        TTF_CloseFont(font);
    }

    void Render()
    {
        SDL_RenderCopy(MainRenderer, texture, NULL, this);
    }

    ~TextureRect()
    {
        SDL_DestroyTexture(texture);
    }

    friend class TextBox;
    friend class Button;
};

class Button : public Object
{
private:
    TextureRect image;
    char text[10]={'\0'};
public:
    SDL_Color Color;

    Button() {};

    // Constructor for text button
    Button(int x, int y, int width, int height, const char* text, const char* Font, SDL_Color ButtonColor, int FontSize = 18, SDL_Color FontColor = { 0,0,0,255 })
    {
        this->x = x;
        this->y = y;
        this->w = width;
        this->h = height;
        
        image.SetText(text, Font, FontSize, FontColor);
        image.x = x + (width - image.w) / 2;
        image.y = y + (height - image.h) / 2;
        Color = ButtonColor;
    }

    // Constructor for image button
    Button(int x, int y,int width,int height, const char* ImagePath, SDL_Color ButtonColor, bool Trancparent = false)
    {
        this->x = x;
        this->y = y;
        this->w = width;
        this->h = height;

        image.SetImage(ImagePath,Trancparent);

        image.x = x + (width - image.w) / 2;
        image.y = y + (height - image.h) / 2;
        Color = ButtonColor;
    }

    // Setting text for button
    void Set(int x, int y, int width, int height, const char* text, const char* Font,SDL_Color ButtonColor, int FontSize = 18, SDL_Color FontColor = { 0,0,0,255 })
    {
        this->x = x;
        this->y = y;
        this->w = width;
        this->h = height;
        short i = 0;
        
        image.SetText(text, Font, FontSize, FontColor);
        image.x = x + (width - image.w) / 2;
        image.y = y + (height - image.h) / 2;
        Color = ButtonColor;

        do {
            this->text[i] = text[i];
            i++;
        } while (text[i] != '\0' && i < 10);
    }
    
    // Setting image for button
    void Set(int x, int y, int width, int height, const char* ImagePath, SDL_Color ButtonColor, bool Trancparent = false)
    {
        this->x = x;
        this->y = y;
        this->w = width;
        this->h = height;

        image.SetImage(ImagePath, Trancparent);

        image.x = x + (width - image.w) / 2;
        image.y = y + (height - image.h) / 2;
        Color = ButtonColor;
    }

    void SetButtonColor(SDL_Color ButtonColor)
    {
        Color = ButtonColor;
    }

    void SetImage(const char *ImagePath)//setting image of Button regarding it's rectange
    {
        SDL_Surface* img = SDL_LoadBMP(ImagePath);
        image.ChangeTexture(img);
        SDL_FreeSurface(img);
    }

    void SetText(const char* Font, const char* text, int FontSize=18, SDL_Color FontColor = { 0,0,0,255 })
    {
        image.SetText(text, Font, FontSize, FontColor);
        image.x = x + (w - image.w) / 2;
        image.y = y + (h - image.h) / 2;
    }

    void Render()
    {
        image.x = x + (w - image.w) / 2;
        image.y = y + (h - image.h) / 2;
        SDL_SetRenderDrawColor(MainRenderer, Color.r, Color.g, Color.b, Color.a);
        SDL_RenderFillRect(MainRenderer, this);
        image.Render();
    }

    void Click()
    {
        SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r, ScreenColor.g, ScreenColor.b, ScreenColor.a);
        SDL_RenderFillRect(MainRenderer, this);
        SDL_Rect rec{ this->x + 5, this->y + 5,w - 10, h - 10 };

        SDL_SetRenderDrawColor(MainRenderer, Color.r, Color.g, Color.b, Color.a);
        SDL_RenderFillRect(MainRenderer, &rec);
        
        rec = image;
        rec.y += 2; rec.w -= 4;
        rec.x += 2; rec.h -= 4;
        SDL_RenderCopy(MainRenderer, image.texture, NULL, &rec);

        SDL_RenderPresent(MainRenderer);
        SDL_Delay(150);

        this->Render();
        SDL_RenderPresent(MainRenderer);
    }

    const char* GetText()
    {
        return text;
    }

    ~Button()
    {
        image.~TextureRect();
    }
};

class ContactButton :public Object
{
private:
    TextureRect NameImage;
    TextureRect NumberImage;
    Person* person;

public:
    int index = 0;
    static TTF_Font* font;
    static SDL_Color FontColor;
    static SDL_Color ButtonColor;
    static TextureRect Icon;

    ContactButton() {};

    ContactButton(short x, short y,short width,short height, Person* person)
    {
        this->x = x;
        this->y = y;
        w = width;
        h = height;
        SDL_Surface* img = TTF_RenderText_Solid(font, person->name, FontColor);
        NameImage.Set(img, x + 100, y + 5);
        SDL_FreeSurface(img);

        img = TTF_RenderText_Solid(font, person->number, FontColor);
        NumberImage.Set(img, x + 100, NameImage.y + NameImage.h + 10);
        SDL_FreeSurface(img);
    }

    void Set(short x, short y,short width,short height, Person* person)
    {
        this->x = x;
        this->y = y;
        w = width;
        h = height;
        SDL_Surface* img = TTF_RenderText_Solid(font, person->name, FontColor);
        NameImage.Set(img, x + 100, y + 5);
        SDL_FreeSurface(img);

        img = TTF_RenderText_Solid(font, person->number, FontColor);
        NumberImage.Set(img, x + 100, NameImage.y + NameImage.h + 10);
        SDL_FreeSurface(img);

    }

    void SetPerson(Person* person)
    {
        this->person = person;
        Update();
    }

    void Click()
    {
        SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r, ScreenColor.g, ScreenColor.b, ScreenColor.a);
        SDL_RenderFillRect(MainRenderer, this);

        SDL_Rect rec{ x + 7, y + 7,w - 14, h - 14 };
        SDL_SetRenderDrawColor(MainRenderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a);
        SDL_RenderFillRect(MainRenderer, &rec);
        
        NumberImage.Render();
        NameImage.Render();
        
        SDL_RenderPresent(MainRenderer);
        SDL_Delay(150);

        this->Render();
        SDL_RenderPresent(MainRenderer);
    }

    void Update()
    {

        SDL_Surface* img = TTF_RenderText_Solid(font, person->name, FontColor);
        NameImage.Set(img, x + 100, y + 5);
        SDL_FreeSurface(img);

        img = TTF_RenderText_Solid(font, person->number, FontColor);
        NumberImage.Set(img, x + 100, NameImage.y + NameImage.h + 10);
        SDL_FreeSurface(img);
    }

    void Render()
    {
        SDL_SetRenderDrawColor(MainRenderer, ButtonColor.r, ButtonColor.g, ButtonColor.b, ButtonColor.a);
        SDL_RenderFillRect(MainRenderer, this);
        NameImage.x = x + 70; NameImage.y = y + 5;
        NumberImage.x = x + 70;NumberImage.y=NameImage.y + NameImage.h + 10;
        Icon.x = x + 20;
        Icon.y=y+20;
        Icon.Render();
        NameImage.Render();
        NumberImage.Render();
    }
    ~ContactButton()
    {
        NumberImage.~TextureRect();
        NameImage.~TextureRect();
    }
};

TTF_Font* ContactButton::font=nullptr;
SDL_Color ContactButton::FontColor = { 0,0,0,0 };
SDL_Color ContactButton::ButtonColor = { 0,0,0,0 };
TextureRect ContactButton::Icon;

class TextBox :public Object
{
private:
    char text[36];
    short TextLength=0;
    TextureRect Image;
    TTF_Font* font=nullptr; 
    SDL_Color BoxColor;
    SDL_Color FontColor;
public:
    // To Read only numbers 
    bool OnlyNumber = false;
    
    TextBox() {}

    TextBox(int x, int y, int windth, int height, const char* Font, int FontSize, SDL_Color BoxColor, SDL_Color FontColor)
    {
        this->font = TTF_OpenFont(Font,FontSize);
        this->x = x; this->y = y;
        w = windth; h = height;
        this->font = font;
        Image.x = x + 5;
        Image.y = y + 5;
        this->BoxColor = BoxColor;
        this->FontColor = FontColor;
    }

    void Set(int x, int y, int windth, int height, const char* Font, int FontSize, SDL_Color BoxColor, SDL_Color FontColor)
    {
        this->font = TTF_OpenFont(Font, FontSize);
        this->x = x; this->y = y;
        w = windth; h = height;
        this->font = font;
        Image.x = x + 5;
        this->BoxColor = BoxColor;
        this->FontColor = FontColor;
    }

    void Clear()
    {
        TextLength = 0;
        text[0] = '\0';
    }

    void Update()
    {
        SDL_Surface* img = TTF_RenderText_Solid(font, text, FontColor);
        Image.ChangeTexture(img);
        SDL_FreeSurface(img);
        Render();
    }

    void Type(const char letter)
    {
        if (letter == '\0' && TextLength > 0)
        {
            text[--TextLength] = '\0';
            Update();
            return;
        }
        if (OnlyNumber && (letter < 48 || letter>57))
            return;
        if (Image.w >= w - 20||letter=='\0') return;

        text[TextLength++] = letter;
        text[TextLength] = '\0';
        Update();
    }

    void SetText(const char* text)
    {
        short i = 0;
        do
            this->text[i] = text[i];
        while (text[i++] != '\0');
        TextLength = i-1;
    }

    const short Length()
    {
        return TextLength;
    }

    char* GetText()
    {
        return text;
    }

    void Render()
    {
        SDL_SetRenderDrawColor(MainRenderer, BoxColor.r, BoxColor.g, BoxColor.b, BoxColor.a);
        SDL_RenderFillRect(MainRenderer, this);
        Image.y = y + (h - Image.h) / 2;
        Image.Render();
    }

    ~TextBox()
    {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }
        Image.~TextureRect();
    }
};

void DrawRect(int x,int y,int width,int height,SDL_Color color)
{
    SDL_SetRenderDrawColor(MainRenderer,color.r,color.g,color.b,color.a);
    SDL_Rect temp{ x, y, width, height };
    SDL_RenderFillRect(MainRenderer, &temp);
}

void SDL_RenderFillCircle(SDL_Renderer* MainRenderer, int x, int y, int radius, SDL_Color c)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    SDL_SetRenderDrawColor(MainRenderer, c.r, c.g, c.b, c.a);
    while (offsety >= offsetx) {

        SDL_RenderDrawLine(MainRenderer, x - offsety, y + offsetx,
            x + offsety, y + offsetx);
        SDL_RenderDrawLine(MainRenderer, x - offsetx, y + offsety,
            x + offsetx, y + offsety);
        SDL_RenderDrawLine(MainRenderer, x - offsetx, y - offsety,
            x + offsetx, y - offsety);
        SDL_RenderDrawLine(MainRenderer, x - offsety, y - offsetx,
            x + offsety, y - offsetx);


        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }
}

#endif // !_Objects_