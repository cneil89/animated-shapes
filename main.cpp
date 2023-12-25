#include <iostream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>

#include <SFML/Graphics.hpp>
#include <windows.h>
#include "imgui.h"
#include "imgui-SFML.h"

void debug(const std::string& msg)
{
    std::cout << msg << std::endl;
}

class LoadedShape
{
public:
    LoadedShape(std::shared_ptr<sf::Shape> shape, const std::string& name, std::shared_ptr<sf::Text> text,
                const sf::Vector2f& pos, const sf::Vector2f& speed, const sf::Vector3i& rgb, float rad) 
        : m_Shape(shape), m_Name(name), m_Text(text), m_Pos(pos), m_Speed(speed), m_RGB(rgb), m_Radius(rad)
    {
        initalizeShape();
    }

    LoadedShape(std::shared_ptr<sf::Shape> shape, const std::string& name, std::shared_ptr<sf::Text> text, 
                const sf::Vector2f& pos, const sf::Vector2f& speed, const sf::Vector3i& rgb, const sf::Vector2f& size) 
        : m_Shape(shape), m_Name(name), m_Text(text), m_Pos(pos), m_Speed(speed), m_RGB(rgb), m_Size(size)
    {
        initalizeShape();
    }

    void updateShape(const sf::RenderWindow& window)
    {
        // basic animation - move the each frame if it's still in frame
        computeShapePosition();
        boundsCheck(window);
        setTextPosition();
        m_Shape->setFillColor(sf::Color(m_RGB.x, m_RGB.y, m_RGB.z));

    }

    // convert m_RGB to floats to use with IMGUI
    sf::Vector3f getFloatColor()
    {
       sf::Vector3f color;
       color.x = m_RGB.x / 225.0f; 
       color.y = m_RGB.y / 225.0f; 
       color.z = m_RGB.z / 225.0f; 
       return color;
    }

    void setColor(const float c[3])
    {
        m_RGB.x = c[0] * 225;
        m_RGB.y = c[1] * 225;
        m_RGB.z = c[2] * 225;
    }

    std::shared_ptr<sf::Shape> getShape() { return m_Shape; }
    std::shared_ptr<sf::Text> getText() { return m_Text; }
    const std::string& getName() const { return m_Name; }

private:
    std::string m_Name;
    std::shared_ptr<sf::Shape> m_Shape;
    std::shared_ptr<sf::Text> m_Text;
    sf::Vector2f m_Pos,m_Speed;
    sf::Vector3i m_RGB;
    sf::Vector2f m_Size;
    float m_Radius;

    void initalizeShape() 
    {
        debug("INIT SHAPE -> " + m_Name);
        m_Shape->setPosition(m_Pos);
        m_Shape->setFillColor(sf::Color(m_RGB.x, m_RGB.y, m_RGB.z));
        setTextPosition();
    }

    void setTextPosition()
    {
        sf::FloatRect shapeBounds = m_Shape->getLocalBounds();
        sf::FloatRect textBounds = m_Text->getLocalBounds();
        m_Text->setOrigin(textBounds.left + textBounds.width / 2, textBounds.top + textBounds.height / 2);
        m_Text->setPosition(m_Pos.x + (shapeBounds.width / 2), m_Pos.y + (shapeBounds.height / 2));
    }

    void computeShapePosition()
    {
        // check for intersect with window border
        m_Pos.x = m_Shape->getPosition().x + m_Speed.x;
        m_Pos.y = m_Shape->getPosition().y + m_Speed.y;
        m_Shape->setPosition(m_Pos);
    }

    void boundsCheck(const sf::RenderWindow& window)
    {
        sf::Vector2u winSize = window.getSize();
        sf::FloatRect shapeBounds = m_Shape->getLocalBounds();

        if (m_Pos.x <= 0 && m_Speed.x < 0)
            m_Speed.x *= -1;
        else if (m_Pos.x + shapeBounds.width >= winSize.x && m_Speed.x > 0)
            m_Speed.x *= -1;
        else if (m_Pos.y <= 0 && m_Speed.y < 0)
            m_Speed.y *= -1;
        else if (m_Pos.y + shapeBounds.height >= winSize.y && m_Speed.y > 0)
            m_Speed.y *= -1;
    }
};

struct GameApp 
{
    GameApp(const std::string& file) 
        : configFilepath(file)
    {
        loadFromFile(configFilepath);

        // attempt to load the font from a file
        if (!font.loadFromFile(fontFilepath))
        {
            // if we can't load the font, print an error to the error console and exit
            std::cerr << "Could not load font!\n";
            exit(-1);
        } 
    }

    void loadFromFile(const std::string& filepath)
    {
        std::ifstream fin(filepath);
        std::string token;

        uint32_t fontSize;
        sf::Vector3i fontRGB;

        std::string name;
        float x, y, sx, sy, w, h, rad;
        int32_t r, g, b;
        sf::Vector2f size;

        while (fin >> token) 
        {
            if (token == "Window")
                fin >> winWidth >> winHeight;
            else if (token == "Font")
                fin >> fontFilepath >> fontSize >> fontRGB.x >> fontRGB.y >> fontRGB.z;
            else if (token == "Circle")
            {
                fin >> name >> x >> y >> sx >> sy >> r >> g >> b >> rad;
                shapes.push_back(std::make_shared<LoadedShape>(std::make_shared<sf::CircleShape>(rad), name, 
                    std::make_shared<sf::Text>(name, font, fontSize), sf::Vector2f(x, y), sf::Vector2f(sx, sy), sf::Vector3i(r, g, b), rad));
            }
            else if (token == "Rectangle")
            {
                fin >> name >> x >> y >> sx >> sy >> r >> g >> b >> size.x >> size.y;
                shapes.push_back(std::make_shared<LoadedShape>(std::make_shared<sf::RectangleShape>(size), name, 
                    std::make_shared<sf::Text>(name, font, fontSize), sf::Vector2f(x, y), sf::Vector2f(sx, sy), sf::Vector3i(r, g, b), size));
            }
        }
        fin.close();
    }

    void draw() 
    {
        for (const auto& shape : shapes)
        {
            if (drawShape)
                m_Window->draw(*(shape->getShape()));
            if (drawText)
                m_Window->draw(*(shape->getText()));
        }
    }

    void setWindow(sf::RenderWindow* window)
    {
        m_Window = window;
        m_Window->setFramerateLimit(60);
    }
    
    sf::RenderWindow* m_Window;
    std::vector<std::shared_ptr<LoadedShape>> shapes;
    uint32_t winWidth, winHeight;
    std::string configFilepath;
    bool drawShape = true;      // whether or not to draw the circle
    bool drawText = true;      // whether or not to draw the text

    // HACK: Move to own class to be cleaner...
    std::string fontFilepath;
    sf::Font font;
};

int main() 
{   
    GameApp game("config.txt");

    // create a new window of size w*h pixels
    // top-left of the window is (0,0) and bottom-right is (w,h)
    // you will have to read these from the config file
    sf::RenderWindow window(sf::VideoMode(game.winWidth, game.winHeight), "SFML works!");
    game.setWindow(&window);

    // initialize IMGUI and create a clock used for its internal timing
    if (!ImGui::SFML::Init(window))
    {
        std::cout << "Error initializing ImGUI-SFML";
    }
    sf::Clock deltaClock;

    // scale the imgui ui by a given factor, does not affect text size
    ImGui::GetStyle().ScaleAllSizes(1.0f);

    // the imgui color {r,g,b} wheel requires floats from 0-1 instead of ints from 0-255
    // set up the text object that will be drawn to the screen
    sf::Text text("Sample Text", game.font, 24);

    // position the top-left corner of the text so that the text aligns on the bottom
    // text character size is in pixels, so move the text up from the bottom by its height
    text.setPosition({0, game.winHeight - (float)text.getCharacterSize()});
    
    // set up a character array to set the text
    char displayString[255] = "Sample Text";

    // main loop - continues for each frame while window is open
    while (window.isOpen())
    {
        // event handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            // pass the event to imgui to be parsed
            ImGui::SFML::ProcessEvent(window, event);

            // this event triggers when the window is closed
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
             
            // this event is triggered when a key is pressed
            if (event.type == sf::Event::KeyPressed)
            {
                // print the key that was pressed to the console
                std::cout << "Key pressed with code = " << event.key.code << "\n";

                if (event.key.code == sf::Keyboard::Escape)
                {
                    // EXIT Program
                    std::cout << "Goodbye!\n";
                    window.close();
                    break;
                }
            }
        }
        
        //Update Frame
        for (const auto& s : game.shapes)
            s->updateShape(window);

        // update imgui for this frame with the time that the last frame took
        ImGui::SFML::Update(window, deltaClock.restart());

        // draw the UI
        ImGui::Begin("Settings");

        static std::shared_ptr<LoadedShape> current_item = nullptr;
        const char* selectableName = current_item != nullptr ? current_item->getName().c_str() : "";

        if (ImGui::BeginCombo("Select Shape##comboSelect", selectableName))
        {
            for (auto& item : game.shapes)
            {
                bool is_selected = (current_item == item);
                if (ImGui::Selectable(item->getName().c_str(), is_selected))
                    current_item = item;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (current_item != nullptr)
        {

            sf::Vector3f shapeRGB = current_item->getFloatColor();
            float c[3] = {shapeRGB.x, shapeRGB.y, shapeRGB.z};
            
            ImGui::Text("Window Text!");
            ImGui::Checkbox("Draw Circle", &game.drawShape);
            ImGui::SameLine();
            ImGui::Checkbox("DrawText", &game.drawText);
            ImGui::ColorEdit3("Color Circle", c);
            current_item->setColor(c); 
        }

        ImGui::Separator();
        ImGui::InputText("Text", displayString, 255);
        if (ImGui::Button("Set Text"))
        {
            text.setString(displayString);
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Circles"))
        {
            for (auto& s : game.shapes)
                s->getShape()->setPosition(0,0);
        }
        ImGui::End();

        window.clear();
        game.draw();
        window.draw(text);
        ImGui::SFML::Render(window); // draw the ui last so it's on top
        window.display();
    }

    return EXIT_SUCCESS;
}