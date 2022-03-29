#include<iostream>
#include<math.h>
#include<SFML\Graphics.hpp>
#include<complex>

using namespace std;

int iterations(complex<double> c) {
	complex<double> z(0, 0);
	int count = 0;
	while (abs(z) < 2 && count < 80) {
		z = (z * z) + c;
		count += 1;
	}
	return count;
}

sf::VertexArray make_set(float scale, float width, float height, float xShift, float yShift) {
	sf::VertexArray set;
	set.setPrimitiveType(sf::PrimitiveType::Points);
	float resolution = .001 / scale;
	float horizontalStart = -1.25;
	float horizontalEnd = -.75;
	float verticalStart = -.5;
	float verticalEnd = .5;
	float size = (height / 4) * scale;
	float centerX = (width / 2);
	float centerY = (height / 2);
	float x;
	float y;
	for (float t = horizontalStart; t < horizontalEnd; t += resolution) {

		for (float m = verticalStart; m < verticalEnd; m += resolution) {

			x = (t * size + centerX);
			y = (m * size + centerY);

			if (x + xShift >= 0 and x + xShift <= 1920 and y - yShift >= 0 and y - yShift <= 1080) {

				complex<double> c(t, m);
				int num = iterations(c);

				if (num < 20) {
					set.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(num * 8, num * 6, num * 12)));
				}
				else if (num < 40) {
					set.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(num * 2, num / 2, num * 6)));
				}
				else if (num == 80) {
					set.append(sf::Vertex(sf::Vector2f(x, y), sf::Color::White));
				}
				else {
					set.append(sf::Vertex(sf::Vector2f(x, y), sf::Color(num * 3, num / 2, num * 2)));
				}

			}
		}
	}
	return set;
}

int main() {
	//RENDER SETUP----------------------------------------------------------
	sf::RenderWindow window(sf::VideoMode(1920, 1080), "Mandelbrot Set", sf::Style::Fullscreen);
	window.setFramerateLimit(144);
	sf::RenderTexture renderSet;
	renderSet.create(1920, 1080);
	sf::Sprite MandelbrotSet(renderSet.getTexture());

	//VARIABLES-------------------------------------------------------------
	sf::Color bgColor = sf::Color(8, 6, 12);
	double scale = 1;
	float xShift = 0;
	float yShift = 0;
	sf::Event event;


	//INITIAL SET CREATION---------------------------------------------------
	renderSet.draw(make_set(scale, window.getSize().x, window.getSize().y, xShift, yShift));
	cout << scale << endl;

	//GAME LOOP--------------------------------------------------------------
	while (window.isOpen()) {
		while (window.pollEvent(event))
		{
			// Close window: exit--------------------------------------------------------------------------------------------------------------------------
			if (event.type == sf::Event::Closed)
				window.close();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
				window.close();
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				if (event.mouseWheelScroll.delta > 0) {
					scale += .1;
					renderSet.clear(bgColor);
					renderSet.draw(make_set(scale, window.getSize().x, window.getSize().y, xShift, yShift));
					cout << scale << endl;
				}
				else if (event.mouseWheelScroll.delta < 0) {
					if (scale - .1 > 0)
						scale -= .1;
					renderSet.clear(bgColor);
					renderSet.draw(make_set(scale, window.getSize().x, window.getSize().y, xShift, yShift));
					cout << scale << endl;
				}
			}

		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			MandelbrotSet.move(0, -3);
			yShift -= 3;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			MandelbrotSet.move(0, 3);
			yShift += 3;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
			MandelbrotSet.move(-3, 0);
			xShift -= 3;
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			MandelbrotSet.move(3, 0);
			xShift += 3;
		}

		//RENDER--------------------------------------------------------------
		window.clear(bgColor);
		window.draw(MandelbrotSet);
		window.display();
	}

}