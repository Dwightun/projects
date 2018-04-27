#include<SFML/Graphics.hpp>
#include<iostream>
#include<math.h>
#include<cstdio>
#include<cstdlib>
#include<list>

#define SIZE_X 1000
#define SIZE_Y 1000
#define G      0.08
#define speed  0.1
#define border 250

class Object
{
protected:
	sf::Vector2f velocity;
	sf::Sprite object;

public:
	// c-tor and d-tor
	Object();
	Object(const sf::Vector2f& vel, const sf::Texture& texture, const sf::Vector2f scale) :
		velocity(vel), object(texture) {};
	~Object() {};

	// methods
	sf::Vector2f get_velocity() { return velocity; }
	sf::Sprite get_object() { return object; }
	void draw(sf::RenderWindow& window);
};

class Bird : public Object
{
public:
	Bird();
	Bird(const sf::Vector2f& vel, const sf::Texture& texture, const sf::Vector2f scale) : Object(vel, texture, scale)
	{
		object.setOrigin(1, 1);
		object.setPosition(SIZE_X / 2, SIZE_Y / 2);
	};
	~Bird() {};

	void fly();
	void apply_force(sf::Time& time);
	float is_collision();
};

class Tree : public Object
{
public:
	Tree();
	Tree(const sf::Vector2f& vel, const sf::Texture& texture, sf::Vector2f scale, float coord_y) : Object(vel, texture, scale)
	{
		object.scale(scale.x, scale.y);
		object.setPosition(SIZE_X, coord_y);
	};
	~Tree() {};

	void move();
	bool check();
};

void Object::draw(sf::RenderWindow& window)
{
	window.draw(this->get_object());
}

void Tree::move()
{
	object.move(sf::Vector2f(velocity.x, 0));
}

void Bird::apply_force(sf::Time& time)
{
	auto t = time.asSeconds();
	velocity.y = G * t;
	object.move(0, velocity.y * t / 2);
}

void Bird::fly()
{
	velocity.y = -15;
	object.move(0, velocity.y * 1 / 2);
}

bool Tree::check()
{
	auto x_coord = object.getPosition().x + border;
	if (x_coord < 0 || x_coord > SIZE_X + border)
		return true;
	return false;
}

void draw_trees(std::vector<Tree>& trees, sf::RenderWindow& window)
{
	if (trees.empty())
		return;
	for (auto& it : trees)
		it.draw(window);
}

void move_trees(std::vector<Tree>& trees)
{
	if (trees.empty())
		return;
	for (auto& it : trees)
		it.move();
}

void delete_trees(std::vector<Tree>& trees)
{
	if (trees.empty())
		return;
	for (auto& it : trees)
	{
		if (it.check())
			trees.pop_back();
	}
}

float myrandom(float min, float max)
{
	return (float)(rand()) / RAND_MAX * (max - min) + min;
}

class MovableBackground // hui
{
public:
	MovableBackground();
	void Update(sf::RenderWindow &window, float elapsedTime);
	void Render(sf::RenderWindow &window);

private:
	sf::Texture bgTex;
	sf::Texture bg2Tex;

	sf::RectangleShape bgShape;
	sf::Vector2f bgSize;

	sf::Sprite      bgSprite;
	sf::Sprite      bg2Sprite;

	float bgSpeed;
	float bgX;
	float bg2X;
	float windowbg1diff;
};

MovableBackground::MovableBackground() // hui
{
	bgSpeed = 0.03;

	bgTex.loadFromFile("background.jpg");
	bgTex.setSmooth(false);
	bgTex.setRepeated(true);

	bgX = bgShape.getPosition().y;
	bgSize.x = SIZE_X*1.5;
	bgSize.y = SIZE_Y;

	bgShape.setTexture(&bgTex);
	bgShape.setTextureRect(sf::IntRect(0, 0, SIZE_X*1.5, SIZE_Y));
	bgShape.setSize(bgSize);
}


void MovableBackground::Update(sf::RenderWindow &window, float elapsedTime) // hui
{
	if (bgX < SIZE_X)
	{
		bgX =(bgX + bgSpeed * elapsedTime);
		if (bgX >= (SIZE_X / 2)) bgX = 0;
	}
	else
	{
		bgX = 0;
	}
	bgShape.setPosition(-bgX, 0);
}
void MovableBackground::Render(sf::RenderWindow &window)
{
	window.draw(bgShape);
}


int main()
{
	sf::RenderWindow window(sf::VideoMode(SIZE_X, SIZE_Y), "My window");
	sf::Texture texture_bird;
	sf::Texture texture_tree_up;
	sf::Texture texture_tree_down;
	sf::Event event;
	sf::Clock clock;
	sf::Time time;
	std::vector<Tree> trees;

	sf::Sprite background;
	sf::Texture backtext;

	MovableBackground bg;



	texture_bird.loadFromFile("Bird-32.png");
	texture_tree_up.loadFromFile("tree_up.png");
	texture_tree_down.loadFromFile("tree_down.png");
	auto bird = new Bird(sf::Vector2f(0, 0), texture_bird, sf::Vector2f(0.1, 0.1));
	auto tree_up = new Tree(sf::Vector2f(-0.1, 0), texture_tree_up, sf::Vector2f(1, 1), 0);
	auto tree_down = new Tree(sf::Vector2f(-0.1, 0), texture_tree_down, sf::Vector2f(1, 1), 650);
	while (window.isOpen())
	{
		auto scale = myrandom(0.9, 1.5);
		auto scale_ = (SIZE_Y - 20 - scale * 400) / 400;
		// auto tree_up = new Wall(SIZE_X, 0, sf::Vector2f(0, 0), texture_wall_up, sf::Vector2f(1, scale));
		// auto tree_down = new Wall(SIZE_X, 650, sf::Vector2f(0, 0), texture_wall_down, sf::Vector2f(1, scale_));
		if (trees.empty())
		{
			trees.push_back(*tree_down);
			trees.push_back(*tree_up);
		}

		//window.clear(sf::Color::Cyan);

		bg.Render(window); // hui
		bg.Update(window, speed * 10); // hui

		bird->draw(window);
		draw_trees(trees, window);
		time = clock.getElapsedTime();
		bird->apply_force(time);
		move_trees(trees);



		while (window.pollEvent(event))
		{
			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::KeyPressed:
				if (event.key.code == sf::Keyboard::Space)
				{
					clock.restart();
					bird->fly();
				}
				break;
			default:
				break;
			}
		}

		delete_trees(trees);
		window.display();
	}
	return 0;
}