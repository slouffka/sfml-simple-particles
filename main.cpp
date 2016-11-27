#include <SFML/Graphics.hpp>

#include <random>

namespace
{
    std::default_random_engine createRandomEngine()
    {
        auto seed = static_cast<unsigned long>(std::time(nullptr));
        return std::default_random_engine(seed);
    }

    auto RandomEngine = createRandomEngine();
}

int randomInt(int exclusiveMax)
{
    std::uniform_int_distribution<> distr(0, exclusiveMax - 1);
    return distr(RandomEngine);
}

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
    public:
        ParticleSystem(unsigned int count)
        : mParticles(count)
        , mVertices(sf::Points, count)
        , mLifetime(sf::seconds(3))
        , mPosition(0, 0)
        {
        }

        void setPosition(sf::Vector2f position)
        {
            mPosition = position;
        }

        void update(sf::Time elapsed)
        {
            for (std::size_t i = 0; i < mParticles.size(); ++i)
            {
                Particle& p = mParticles[i];
                p.lifetime -= elapsed;

                if (p.lifetime <= sf::Time::Zero)
                    resetParticle(i);

                mVertices[i].position += p.velocity * elapsed.asSeconds();

                float ratio = p.lifetime.asSeconds() / mLifetime.asSeconds();
                mVertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
            }
        }

    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
        {
            states.transform *= getTransform();
            states.texture = NULL;

            target.draw(mVertices, states);
        }

    private:
        struct Particle
        {
            sf::Vector2f velocity;
            sf::Time lifetime;
        };

        void resetParticle(std::size_t index)
        {
            float angle = (std::rand() % 360) * 3.14f / 180.f;
            float speed = (std::rand() % 50) + 50.f;
            mParticles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
            mParticles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

            // color
            sf::Color newColor(randomInt(255), randomInt(255), randomInt(255));
            mVertices[index].color = newColor;

            // position
            mVertices[index].position = mPosition;
        }

    private:
        std::vector<Particle>   mParticles;
        sf::VertexArray         mVertices;
        sf::Time                mLifetime;
        sf::Vector2f            mPosition;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Particles", sf::Style::Close);
    ParticleSystem particles(10000);

    sf::Clock clock;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // handle window events
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            // handle keys pressed
            if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Escape) {
                    window.close();
                }
            }

        }

        sf::Vector2i mouse = sf::Mouse::getPosition(window);
        particles.setPosition(window.mapPixelToCoords(mouse));

        sf::Time elapsed = clock.restart();
        particles.update(elapsed);

        window.clear();
        window.draw(particles);
        window.display();
    }

    return 0;
}
