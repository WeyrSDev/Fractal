#include "Application.hpp"

#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Sleep.hpp>

#include <cmath>
#include <sstream>

Application::Application() :
    m_window(sf::VideoMode(1000, 765), "Fractal - Mandelbrot"),//, sf::Style::Titlebar | sf::Style::Close),
    m_fractal(sf::Vector2u(1000, 765), 4),
    m_down(-1, -1),
    m_resized(true)
{
    m_window.setFramerateLimit(60);

    // Init select frame
    m_select.setFillColor(sf::Color(0, 0, 0, 0));
    m_select.setOutlineColor(sf::Color(255, 255, 255, 255));
    m_select.setOutlineThickness(-2.f);

    m_font.loadFromFile("DejaVuSans.ttf");
    m_precision.setFont(m_font);
    m_precision.setPosition(10.f, 10.f);
    m_precision.setCharacterSize(20.f);
    std::stringstream ss;
    ss << std::fixed << static_cast<int>(m_fractal.precision());
    m_precision.setString(ss.str());

    // Init fractal
    paused(true);
    m_fractal.update(sf::Vector2i(0, 0), static_cast<sf::Vector2i>(m_window.getSize()));
}

void Application::run()
{
    while(m_window.isOpen())
    {
        update();
        draw();
    }
}

void Application::update()
{
    sf::Event event;
    while(m_window.pollEvent(event))
    {
        // Ignore the events happend before/while resizing
        if(m_resized)
            continue;

        // Pass events to SFGUI
        //m_desktop.HandleEvent(event);

        if(event.type == sf::Event::Closed)
            m_window.close();
        // Set select frame
        else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
        {
            m_down.x = event.mouseButton.x;
            m_down.y = event.mouseButton.y;
        }
        // Update
        else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left)
        {
            // Feedback for user
            paused();

            // Update fractal
            m_fractal.update(m_down, sf::Vector2i(event.mouseButton.x, event.mouseButton.y));

            // No select frame
            m_down.x = -1;
            m_down.y = -1;
        }
        else if(event.type == sf::Event::KeyPressed)
        {
            if(event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Down)
            {
                if(event.key.code == sf::Keyboard::Up)
                    m_fractal.precision(m_fractal.precision()+10);
                else if(event.key.code == sf::Keyboard::Down)
                    m_fractal.precision(m_fractal.precision()-10);

                std::stringstream ss;
                ss << std::fixed << static_cast<int>(m_fractal.precision());
                m_precision.setString(ss.str());
            }
        }
        else if(event.type == sf::Event::KeyReleased)
        {
            if(event.key.code == sf::Keyboard::Return)
            {
                paused();
                m_fractal.update(sf::Vector2i(0, 0), static_cast<sf::Vector2i>(m_window.getSize()));
            }
        }
        else if(event.type == sf::Event::Resized)
            onResize();
    }
    m_resized = false;

    // Update SFGUI with elapsed seconds since last call.
    //m_desktop.Update(m_frametime.restart().asSeconds());

    // Change select frame
    if(m_down.x != -1 && m_down.x != -1)
    {
        m_select.setPosition(static_cast<sf::Vector2f>(m_down));
        m_select.setSize(static_cast<sf::Vector2f>(sf::Mouse::getPosition(m_window))-static_cast<sf::Vector2f>(m_down));
    }
    else
        m_select.setSize(sf::Vector2f(0, 0));
}

void Application::draw()
{
    m_window.resetGLStates();

    m_window.clear();
    m_window.draw(m_fractal);
    m_window.draw(m_precision);
    m_window.draw(m_select);
    //m_sfgui.Display(m_window); // Draw SFGUI elements
    m_window.display();
}

void Application::paused(bool fractal)
{
    sf::Text wait;
    wait.setFont(m_font);
    wait.setCharacterSize(30);
    wait.setString("Please Wait!");
    wait.setPosition(static_cast<sf::Vector2f>(m_window.getSize())/2.f-sf::Vector2f(wait.getLocalBounds().width, wait.getLocalBounds().height)/2.f);

    sf::RectangleShape rect;
    rect.setSize(static_cast<sf::Vector2f>(m_window.getSize()));
    rect.setFillColor(sf::Color(10, 10, 10, 220));

    m_window.clear();
    if(fractal)
        m_window.draw(m_fractal);
    m_window.draw(rect);
    m_window.draw(wait);
    m_window.display();
}

void Application::onResize()
{
    m_resized = true;

	sf::Vector2f size = static_cast<sf::Vector2f>(m_window.getSize());

	// Minimum size
	if(size.x < 800)
		size.x = 800;
	if(size.y < 600)
		size.y = 600;

	// Apply possible size changes
	m_window.setSize(static_cast<sf::Vector2u>(size));

	// Reset static (1:1) view
	m_staticView = sf::View(sf::FloatRect(0.f, 0.f, size.x, size.y));
	m_window.setView(m_staticView);

	// Resize widgets

	// The sidebar should be 180px wide
	const float width = 180.f;

	//m_desktop.UpdateViewRect(sf::FloatRect(0.f, 0.f, size.x, size.y));
    //	m_sideBar->SetAllocation(sf::FloatRect(0.f, 0.f, width, size.y));

    // Resize & update the fractal
    m_fractal.resize(static_cast<sf::Vector2u>(size), 4);
    paused();
    m_fractal.update(sf::Vector2i(0, 0), static_cast<sf::Vector2i>(size));
}
