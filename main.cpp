/*
    NOTE!!!!!!!
    Before compiling several libraries should be linked :
    SDL2main.lib
    SDL2.lib
    SDL2_ttf.lib
*/

#include<iostream>
#include<string>
#include<vector>
#include"Header/Objects.h"
//#include "Header/Linked List.h"

//Main data

const int WindowWidth = 400;// screen dimensions:
const int WindowHeight = 650;
char screen = 'C';/// to know which screen is in present: 'N' for numpad , 'C' for contacts, 'E' for editing screen
Object* Focused = nullptr;// Which object is on focus (of mouse)
// Contact Screen objects

namespace CallScreen
{
    Button Volume,End;
};

namespace KeypadScreen
{
    Button Numpad[14];// Numberpad data
    TextBox NumBox;// Text of Numpad screen
};

namespace ContactScreen
{
    std::vector<ContactButton*> Contacts;
    std::vector<Person*> Records;
    TextBox SearchBox;
    Button Edit;
    Button Delete;
    Button Call;
    int Top=0;// Top is used to identify from which contact index contacts should be rendered!
    Object* Objects[] = { &SearchBox,&Edit,&Delete,&Call };
};

// EditScreen Objects
namespace EditScreen
{
    TextBox Name;
    TextBox Number;
    TextBox Email;
    Button Ok, Cancel;
    Object* Objects[] = { &Name,&Number,&Email,&Ok,&Cancel };
    // if postion < 0 then new record is being added , else existing record is beign edited
    int position = -1;
};

// Screen move button
namespace Screen 
{
    Button ToKeyPad,ToContact, AddContact;
}

void WanishButtons()
{
    using namespace ContactScreen;
    EditScreen::position = -1;
    if (Delete.y < 0)return;
    DrawRect(Call.x, Call.y, 200, Call.h, ContactButton::ButtonColor);

    Delete.y = Edit.y = Call.y = -100;
}

void RenderCallScreen(const char* name, const char* number)
{
    SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r, ScreenColor.g, ScreenColor.b, ScreenColor.a);
    SDL_RenderClear(MainRenderer);
    TextureRect Text;
    screen = 'P';
    using namespace CallScreen;
    Text.SetText(name, "Sources/Font.ttf", 24);
    Text.x = (WindowWidth - Text.w) / 2;
    Text.y = 50;
    Text.Render();

    Text.SetText(number, "Sources/Font.ttf", 24);
    Text.x = (WindowWidth - Text.w) / 2;
    Text.y = 100;
    Text.Render();

    Volume.Render();
    End.Render();
}

void RendrContactButtons(int x, int y)
{
    using namespace ContactScreen;
    WanishButtons();
    Call.x = x + 70;

    Edit.x = x + 120;

    Delete.x = x + 170;

    Delete.y = Edit.y = Call.y = y + 65;


    Edit.Render();
    Call.Render();
    Delete.Render();
}

//To Dispay Screeen switching buttons
void RenderScreenMoveButtons()
{
    using namespace Screen;
    DrawRect(0, WindowHeight - 50, WindowWidth, 70, { 100,100,100,255 });
    ToContact.Render();
    ToKeyPad.Render();
    AddContact.Render();
}

// To Display Contact Editing screen
void RenderEditScreen(Person *person=nullptr)
{
    // After leaving edit screen every thing should be set to default
    ContactScreen::SearchBox.Clear();
    ContactScreen::SearchBox.Update();
    ContactScreen::Contacts.resize(ContactScreen::Records.size());
    ContactScreen::Top = 0;
    Screen::ToContact.Color= { 100,100,250,255 };
    Screen::ToKeyPad.Color = { 100, 200, 100, 255 };
    using namespace EditScreen;
    screen = 'E';// switching to Edit screen
    SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r, ScreenColor.g, ScreenColor.b, ScreenColor.a);
    SDL_RenderClear(MainRenderer);
    TextureRect Text;
    Text.x = 8;
    
    Name.Clear();
    Number.Clear();
    Email.Clear();

    if (person)
    {
        Name.SetText(person->GetName());
        Number.SetText(person->GetNumber());
        Email.SetText(person->GetEmail());
    }

    Text.SetText("Name", "Sources/Font.ttf", 24);
    int h = Text.h+10;
    Text.y = Name.y - h;
    Text.Render();

    Name.Update();
    
    Text.SetText("Telephone number","Sources/Font.ttf",24);
    Text.y = Number.y - h;
    Text.Render();
    Number.Update();

    Text.SetText("Email", "Sources/Font.ttf", 24);
    Text.y = Email.y - h;
    Text.Render();
    Email.Update();
    Ok.Render();
    Cancel.Render();
}

// To Display Contacts list
void RenderContactScreen()
{
    using namespace ContactScreen;
    WanishButtons();

    SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r, ScreenColor.g, ScreenColor.b, ScreenColor.a);
    SDL_RenderClear(MainRenderer);
    screen = 'C';

    int size = Contacts.size();

    if (Top >= size) {
        DrawRect(0, 0, WindowWidth, 60, { 100,100,100,255 });
        SearchBox.Render();
        Screen::AddContact.Render();
        RenderScreenMoveButtons();
        return;
    }

    ContactButton* contact = Contacts[Top];
    int b = abs((contact->y - 70) / (contact->h + 10))+Top;
    int e = WindowHeight / (contact->h + 10) + b + 1;
    if (e > size)
        e = size;
    int y = (b-Top) * (contact->h + 10) + contact->y;

    while (b < e)
    {
        contact = Contacts[b];
        contact->y = y;
        contact->Render();
        y += contact->h + 10;
        b++;
    }
    
    DrawRect(0, 0, WindowWidth, 60, { 100,100,100,255 });
    SearchBox.Render();
    Screen::AddContact.Render();
    RenderScreenMoveButtons();
}

void RenderKeypadScreen()
{
    
    using namespace KeypadScreen;
    screen = 'K';

    SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r, ScreenColor.g, ScreenColor.b, ScreenColor.a);
    SDL_RenderClear(MainRenderer);
    NumBox.Update();
    for (auto& b : Numpad)
        b.Render();
    RenderScreenMoveButtons();
}

bool Compare(const char* text1, const char* text2)
{
    int i = 0;
    while (text1[i] != '\0' && text2[i] != '\0')
    {
        if (text1[i] != text2[i]&& abs(text1[i] - text2[i])!=32)
            return false;
        i++;
    }
    return true;
}

void Search()
{
    using namespace ContactScreen;
    Top = Records.size();
    Contacts.resize(Records.size());



    for (int i = Records.size()-1; i >-1 ; i--)
    {
        Person* p = Records[i];
        if (Compare(p->GetName(), SearchBox.GetText()))
        {
            Contacts[i]->y = 70;
            Contacts.push_back(Contacts[i]);
        }
    }
}

// To Initialzie Contacts Screen Objects
void InitializeEditScreen()
{
    using namespace EditScreen;

    Name.Set(8, 50, WindowWidth - 16, 40, "Sources/Font.ttf", 24, { 230,230,230,255 }, { 0,0,0,255 });
    Number.Set(8, 150, WindowWidth - 16, 40, "Sources/Font.ttf", 24, { 230,230,230,255 }, { 0,0,0,255 });
    Email.Set(8, 250, WindowWidth - 16, 40, "Sources/Font.ttf", 18, { 230,230,230,255 }, { 0,0,0,255 });
    Ok.Set(50, WindowHeight - 70, 100, 50, "Save", "Sources/Font.ttf", { 100,200,100,255 }, 20);
    Cancel.Set(WindowWidth-150, WindowHeight - 70, 100, 50, "Cancel", "Sources/Font.ttf", { 100,200,100,255 }, 20);

}

// To initialize Contact Screen Objects
void InitializeContacts()
{
    using namespace ContactScreen;
    
    Edit.Set(100, -100, 30, 30, "Sources/edit.bmp", { 100,250,100,250 }, true);
    Call.Set(200, -100, 30, 30, "Sources/call.bmp", { 100,250,100,250 },true);
    Delete.Set(300,-300, 30, 30, "Sources/delete.bmp", { 100,250,100,250 },true);
    SearchBox.Set(8, 8, WindowWidth - 16, 40, "Sources/Font.ttf", 24, { 230,230,230,255 }, { 0,0,0,255 });

    std::fstream file("Header/Contacts.dat", std::ios::binary|std::ios::in|std::ios::app);
    file.seekg(0, std::ios::end);

    ContactButton::font = TTF_OpenFont("Sources/Font.ttf",20);
    ContactButton::FontColor = { 0,0,0,255 };
    ContactButton::ButtonColor = { 200,200,200,0 };
    ContactButton::Icon.SetImage("Sources/icon.bmp", true);

    int len = file.tellg() / sizeof(Person);

    for (int i = 0, y = 70; i < len; i++)
    {
        //initalizing records
        Person* person = new Person;
        person->ReadFromFile(file, i);
        Records.push_back(person);

        //initializing contact button
        ContactButton* button = new ContactButton(10, y, WindowWidth - 20, 100, person);
        button->index = i;
        Contacts.push_back(button);
        y += 110;
    }
    
    if (!len)
    {
        Person* p = new Person;
        p->SetName("Set name");
        p->SetNumber("Set number");
        p->SetEmail("null");
        p->SaveToFile(file,0);
        Records.push_back(p);
        ContactButton* button = new ContactButton(10, 70, WindowWidth - 20, 100, p);
        button->index = 0;
        Contacts.push_back(button);
    }
    file.close();
}

// To initialize Keypad screen

void InitializeKeypad()
{
    using namespace KeypadScreen;
    
    NumBox.Set(8,8, WindowWidth - 16, 50, "Sources/Font.ttf", 28, { 230,230,230,255 }, { 0,0,0,255 });
    int w =(WindowHeight-150)/5-10,dx=(WindowWidth-3*(w)-40)/2,y = WindowHeight - 5 * w-100;
    
    for (int i = 0; i < 3; i++) {
        int x =dx;
        
        for (int j = 0; j < 3; j++)
        {
            char num[2] = { i * 3 + j + 49,'\0' };
            Numpad[i * 3 + j].Set(x, y, w, w, num, "Sources/Font.ttf", { 200,200,200,255 }, 30);
            x += (w + 20);
        }
        y += (w + 20);
    }

    int x = dx;
    Numpad[9].Set(x,y, w, w, "*","Sources/Font.ttf", { 200,200,200,255 }, 30);
    x += w + 20;
    Numpad[10].Set(x,y, w, w, "0", "Sources/Font.ttf", { 200,200,200,255 }, 30);
    x += w + 20;
    Numpad[11].Set(x,y, w, w, "+", "Sources/Font.ttf", { 200,200,200,255 }, 30);
    y += w + 15;
    Numpad[12].Set(x,y  ,80,52, "Sources/backslash.bmp", { 255,255,255,255 }, true);
    Numpad[13].Set(dx, y, w, 52, "Sources/call.bmp", { 100,200,100,255 }, true);
}

// Procedure when MouseScrolling event happens
void OnScroll(short dh)
{
    if (screen == 'C')
    {
        using namespace ContactScreen;

        if (Top >= Contacts.size())return;

        ContactButton* contact = Contacts.back();
        

        if (contact->y + contact->h <= WindowHeight - 60 && dh < 0)
            return;
        
        contact = Contacts[Top];

        if (contact->y >= 70 && dh > 0)
            return;

        SDL_SetRenderDrawColor(MainRenderer, 255, 255, 255, 255);
        SDL_RenderClear(MainRenderer);
        contact->y += dh;
        RenderContactScreen();
        RenderScreenMoveButtons();
    }
}

// Procedure when keyboard button is pressed
void Onkeydown(short key)
{
    if (screen == 'C')
    {
        using namespace ContactScreen;
        if (Focused == &SearchBox && key == 42)
        {
            SearchBox.Type('\0');
            if (SearchBox.Length() == 0)
            {
                Contacts.resize(Records.size());
                Top = 0;
                RenderContactScreen();
                return;
            }
            Search();
            RenderContactScreen();
            return;
        }
        return;
    }

    if (screen == 'E'&&Focused)
    {
        if (Focused->y<400 && key == 42)
        {
            ((TextBox*)Focused)->Type('\0');
            return;
        }
    }
}

// Procedure when Keyboard letters are pressed
void OnTextinput(const char *text)
{

    if (screen == 'C' && Focused == &ContactScreen::SearchBox)
    {
        using namespace ContactScreen;
        /*
        *   Top is used to identify from which contact index contacts should be rendered!
        *   when something typed on Searchbox Search function will push contacts to end of Contacts list so rendering beging from last index 
        */
        SearchBox.Type(text[0]);
        Search();
        RenderContactScreen();
        return;
    }

    if (screen == 'E'&&Focused)
    {
        if (Focused->y < 300)
            ((TextBox*)Focused)->Type(text[0]);
        return;
    }
}

// Deletes spesific record from Record and from Contacts
void DeleteRecord(int position)
{
    using namespace ContactScreen;
    int size = Records.size() - 1; 
    
    if (size == 0)return;// if only one contact remainded its not allowed to delete it

    // if contacts was first contact then contact next to it will be Top contact
    if(position!=size)Contacts[position + 1]->y = Contacts[position]->y;

    delete Records[position];
    delete Contacts[position];

    for (int i=position;i< size;i++ )
    {
        Records[i] = Records[i+1];
        Contacts[i] = Contacts[i+1];
        Contacts[i]->index = i;
    }

    // deleted form records
    Records.resize(size);
    

    // Rewritiong file to delete one spesific record
    remove("Header/Contacts.dat");// file removed
    
    std::fstream file("Header/Contacts.dat", std::ios::binary |std::ios::app);// creating new file
    
    // writing directily from Records to file
    for (int i = 0; i < size; i++)
    {
        Records[i]->SaveToFile(file,i);
    }
    file.close();

    // Everything set to default
    SearchBox.Clear();
    SearchBox.Update();
    Contacts.resize(size);
    Top = 0;
    EditScreen::position = -1;
}

void SaveRecord()
{   
    int index = EditScreen::position;
    Person* p;
    ContactButton* button;
    
    // if position < 0 then new record is added
    if (EditScreen::position < 0)
    {
        p = new Person;
        index = ContactScreen::Records.size();

        ContactButton* last = ContactScreen::Contacts.back();
        button = new ContactButton;
        button->SetRect(last->x, last->y + last->h + 10, last->w, last->h);
        button->index = ContactScreen::Records.size();
        
        ContactScreen::Contacts.push_back(button);
        ContactScreen::Records.push_back(p);
    }
    
    p = ContactScreen::Records[index];
    p->SetName(EditScreen::Name.GetText());
    p->SetNumber(EditScreen::Number.GetText());
    p->SetEmail(EditScreen::Email.GetText());
    
    button = ContactScreen::Contacts[index];    
    button->SetPerson(p);

    // sving to file
    std::fstream file("Header/Contacts.dat", std::ios::binary | std::ios::out | std::ios::in);
    p->SaveToFile(file, EditScreen::position);
    file.close();
}

// Procedure when Mouse buttons are pressed
void OnmouseDown(int x,int y,int mb)
{
    Focused = nullptr;
    if (screen == 'C')
    {
        if (Screen::ToKeyPad.Contain(x, y))
        {
            Screen::ToKeyPad.Color = { 100,100,250,255 };
            Screen::ToContact.Color = { 100,200,100,255 };
            RenderKeypadScreen();
        }

        using namespace ContactScreen;

        if (SearchBox.Contain(x, y))
        {
            Focused = &SearchBox;
            return;
        }
       
        if (Screen::AddContact.Contain(x, y))
        {
            Focused = &Screen::AddContact;
            Screen::AddContact.Click();
            EditScreen::position = -1;
            screen = 'E';
            RenderEditScreen();
            return;
        }
        
        if (Top<Contacts.size() && y > SearchBox.y + SearchBox.h && y < WindowHeight - 50)
        {
            if (Edit.Contain(x, y))
            {
                Edit.Click();
                RenderEditScreen(Records[EditScreen::position]);
                return;
            }
            
            if (Delete.Contain(x, y))
            {
                Delete.Click();
                DeleteRecord(EditScreen::position);
                RenderContactScreen();
                return;
            }

            if (Call.Contain(x, y))
            {
                Call.Click();
                Person* p = Records[EditScreen::position];
                RenderCallScreen(p->GetName(),p->GetNumber());
                return;
            }
            
            ContactButton* b = Contacts[Top];
            int index = (y - b->y) / (b->h + 10)+Top;
            if (index < Contacts.size())
            {
                b = Contacts[index];
                if (b->Contain(x, y))
                {
                    Focused = b;
                    if (b->y < 70)
                        OnScroll(70 - b->y);
                    if (b->y > WindowHeight - b->h-60)
                        OnScroll(WindowHeight - b->y-b->h - 60);

                    b->Click();
                    RendrContactButtons(b->x, b->y);
                    // positon should be set after clling rendercontacbuttons!(after this function postion = -1)
                    EditScreen::position = b->index;
                }
            }
        }
        if (!Focused && ContactScreen::Call.y > 0)
            WanishButtons();
    }
    
    if (screen == 'E')
    {
        using namespace EditScreen;

        for (Object* ob : Objects)
            if (ob->Contain(x, y))
            {
                Focused = ob;
                break;
            }
    
        if (Focused == &Ok)
        {
            Ok.Click();
            // if Name or number is empty then it is not ganne be saved
            if (EditScreen::Name.Length() == 0 || EditScreen::Number.Length() == 0)return;

            SaveRecord();

            RenderContactScreen();
            screen = 'C';
            return;
        }
        if (Focused == &Cancel)
        {
            Cancel.Click();

            RenderContactScreen();
            screen = 'C';
            return;
        }
    }

    if (screen == 'K')
    {
        using namespace KeypadScreen;
        if (Screen::ToContact.Contain(x, y))
        {
            Screen::ToContact.Color = { 100,100,250,255 };
            Screen::ToKeyPad.Color = { 100,200,100,255 };
            RenderContactScreen();
            return;
        }
        
        if (Screen::AddContact.Contain(x, y))
        {
            Person p("\0", NumBox.GetText(), "\0");
            RenderEditScreen(&p);
            return;
        }

        
        if (Numpad[12].Contain(x, y)) {
            NumBox.Type('\0');
            Numpad[12].Click();
            return;
        }
        if (Numpad[13].Contain(x, y)&&NumBox.Length()>0)
        {
            Numpad[13].Click();
            RenderCallScreen("\0", NumBox.GetText());
            return;
        }
        int w = (WindowHeight - 150) / 5 - 10,y0 = WindowHeight - 5 * w - 100;

        if (y < y0||y>WindowHeight-60) return;
        int dx = (WindowWidth - 3 * (w)-40) / 2;

        short index = (x-dx) / (w + 20) + (y - y0) / (w + 20) * 3;

        if (Numpad[index].Contain(x, y))
            Focused = &Numpad[index];
        else return;

        Numpad[index].Click();

        NumBox.Type(Numpad[index].GetText()[0]);
    }

    if (screen == 'P')
    {
        using namespace CallScreen;
        if (End.Contain(x, y))
        {
            End.Click();
            RenderContactScreen();
            screen = 'C';
            return;
        }

        if (Volume.Contain(x, y))
        {           
            if (Volume.Color.g == Volume.Color.r)
                Volume.Color = { 100,200,100,255 };
            else
                Volume.Color = { 200,200,200,255 };
            Volume.Click();
            return;
        }
    }
}

// Initializing all data
void SetUp()
{
    InitializeContacts();
    InitializeEditScreen();
    InitializeKeypad();

    int w = (WindowWidth - 180) / 2;

    Screen::AddContact.Set(WindowWidth - 60, WindowHeight - 45, 40, 40, "+", "Sources/Font.ttf", { 100,250,100,250 }, 38);
    Screen::ToKeyPad.Set(30, WindowHeight - 45, w,40,"Keypad", "Sources/Font.ttf", {100,200,100,255},20);
    Screen::ToContact.Set(60+w, WindowHeight - 45, w, 40, "Contacts", "Sources/Font.ttf", { 100,100,250,255 },20);
    //Edit.Set(100, -100, 30, 30, "Sources/edit.bmp", { 100,250,100,250 }, true);
    CallScreen::End.Set((WindowWidth-50)/2, WindowHeight-100, 50, 50, "Sources/end.bmp", { 255,255,255,255 }, true);
    CallScreen::Volume.Set((WindowWidth - 70) / 2, WindowHeight - 250, 70, 70, "Sources/volume.bmp", { 200,200,200,255 }, true);
    RenderContactScreen();
}

int main(int argc, char* args[])
{
    //initializing SDL
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        std::cout << "Can not load SDL" << std::endl;
    if (TTF_Init() < 0)// it is important to initialize TTF  !!!!!
        std::cout << "Can not load SDL_TTF" << std::endl;

    //creating window
    SDL_Window* window;
    window = SDL_CreateWindow("Phonebook", 100, 100, WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
    
    // creating MainRenderer in the window
    
   
    MainRenderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(MainRenderer, ScreenColor.r,ScreenColor.g,ScreenColor.b,ScreenColor.a);// to set drawing color of MainRenderer ot white
    SDL_RenderClear(MainRenderer);// to clear MainRenderer

    SetUp();
    bool running = true;
    //main loop
  

    while (running)
    {
        
        //  Handle Input
        SDL_Event event;
      
        // Start our event loop to handle each specific event
        while (SDL_PollEvent(&event)) {
            // quite button is pressed
            if (event.type == SDL_QUIT) {
                running = false;
            }
            switch (event.type)
            {
            
            case SDL_MOUSEBUTTONDOWN:
                OnmouseDown(event.button.x, event.button.y, event.button.button);
                break;

            case SDL_TEXTINPUT:
                OnTextinput(event.text.text);
                break;
            
            case SDL_KEYDOWN:
                Onkeydown(event.key.keysym.scancode);
                break;

            case SDL_MOUSEWHEEL:
                OnScroll(event.wheel.y*5);
                break;
            }
           
        }
        SDL_Delay(20);
        SDL_RenderPresent(MainRenderer);
    }

    TTF_CloseFont(ContactButton::font);

    SDL_DestroyRenderer(MainRenderer);
    SDL_DestroyWindow(window);
    
    TTF_Quit();
    SDL_Quit();
    return 0;
}