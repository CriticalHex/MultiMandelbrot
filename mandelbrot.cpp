#include<iostream>
#include<math.h>
#include<SFML\Graphics.hpp>
#include<complex>
#include<thread>
#include<vector>
#include<list>

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


void fill_array(sf::VertexArray& set, float scale, float width, float height, int mouse_x, int mouse_y, int thread, int max_threads) {
	double shiftX;
	double shiftY;
	double horizontalRes;
	double verticalRes;
	double centerX = (width / 2);
	double centerY = (height / 2);
	double horizontalStart = (-2 / scale);
	double horizontalEnd = (2 / scale);
	double verticalStart = (-2 / scale);
	double verticalEnd = (2 / scale);
	double horizontalSize = (width / (abs(horizontalStart) + abs(horizontalEnd)));
	double verticalSize = (height / (abs(verticalStart) + abs(verticalEnd)));

	shiftX = ((centerX) + (((centerX) - (mouse_x))));
	shiftY = ((centerY) + (((centerY) - (mouse_y))));
	horizontalSize = (width / (abs(horizontalStart) + abs(horizontalEnd)));
	verticalSize = (height / (abs(verticalStart) + abs(verticalEnd)));
	horizontalRes = (abs(horizontalStart) + abs(horizontalEnd)) / width * 1;
	verticalRes = (abs(verticalStart) + abs(verticalEnd)) / height * 1;

	horizontalStart = (-2 / scale) + ((abs((-2 / scale)) + abs(horizontalEnd)) / max_threads) * thread;
	horizontalEnd = (-2 / scale) + ((abs((-2 / scale)) + abs(horizontalEnd)) / max_threads) * (thread + 1);

	double x;
	double y;
	int num;
	for (double t = horizontalStart; t < horizontalEnd; t += horizontalRes) {

		for (double m = verticalStart; m < verticalEnd; m += verticalRes) {

			x = (t * horizontalSize + shiftX);
			y = (m * verticalSize + shiftY);

			if (x >= 0 and x <= width and y >= 0 and y <= height) {

				complex<double> c(t,m);

				num = iterations(c);

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
}

void make_set(list<sf::VertexArray> &sets, list<sf::VertexArray>::iterator setIter, list<thread> &active_threads, int max_threads, double scale, int window_width, int window_height, int mouse_x, int mouse_y) {
	int i = 0;
	for (setIter = sets.begin(); setIter != sets.end(); setIter++) {
		(*setIter).clear();
	}
	for (auto& s : sets) {
		active_threads.emplace_back(fill_array, ref(s), scale, window_width, window_height, mouse_x, mouse_y, i, max_threads);
		i++;
	}
}

int main() {
	//RENDER SETUP----------------------------------------------------------
	sf::RenderWindow window(sf::VideoMode(1000, 1000), "Mandelbrot Set", sf::Style::None);
	window.setPosition(sf::Vector2i(460, 20));
	window.setFramerateLimit(60);

	//VARIABLES-------------------------------------------------------------
	sf::Color bgColor = sf::Color(8, 6, 12);
	double scale = 1;
	sf::Event event;
	list<thread> active_threads;
	int max_threads = 4;
	list<sf::VertexArray> sets;
	list<sf::VertexArray>::iterator setIter;

	//INITIAL SET CREATION---------------------------------------------------
	for (int i = 0; i < max_threads; i++) {
		sf::VertexArray s1;
		s1.setPrimitiveType(sf::PrimitiveType::Points);
		sets.push_back(s1);
	}
	make_set(sets, setIter, active_threads, max_threads, scale, window.getSize().x, window.getSize().y, 500, 500);

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
					scale *= 2;
				}
				else if (event.mouseWheelScroll.delta < 0) {
					scale /= 2;
				}
				make_set(sets, setIter, active_threads, max_threads, scale, window.getSize().x, window.getSize().y, event.mouseWheelScroll.x, event.mouseWheelScroll.y);
			}
		}
		/*for (auto& th : active_threads) {
			if (th.joinable())
				th.join();
		}*/
		//RENDER--------------------------------------------------------------
		window.clear(bgColor);
		for (setIter = sets.begin(); setIter != sets.end(); setIter++) {
			window.draw(*setIter);
		}
		window.display();
	}
	for (auto& th : active_threads) {
		if (th.joinable())
			th.join();
	}
}